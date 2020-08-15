#define _GNU_SOURCE
#include "server.h"

char *socketName;
char *outputFile;
extern int numBuckets;
int sockfd;

tecnicofs *fs;

/* Displays an error message. */
static void displayUsage(const char *appName) {
  printf("Usage: %s sock_path output_filepath threads_number\n", appName);
  exit(EXIT_FAILURE);
}

/* Verifies if all arguments exist and are correct. */
static void parseArgs(long argc, char *const argv[]) {
  if (argc != NUMBER_ARGUMENTS) {
    fprintf(stderr, "Invalid format:\n");
    displayUsage(argv[0]);
  }

  socketName = argv[1];
  outputFile = argv[2];
  numBuckets = atoi(argv[3]);

  if (numBuckets < 1) {
    fprintf(stderr, "Invalid number of trees.\n");
    displayUsage(argv[0]);
  }
}

/* Opens the output file and returns it. */
FILE *openOutputFile() {
  FILE *fp = fopen(outputFile, "w");
  checkError(!fp, "Error opening output file");
  return fp;
}

/* Returns the time difference (in seconds) between the given arguments. */
float timeDiff(struct timeval start, struct timeval end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}

/* Displays an error message indicating the line where it occured. */
void errorParse() {
  fprintf(stderr, "Error: invalid line\n");
  exit(EXIT_FAILURE);
}

/* Creates a new table to store all the opens files by connfd. */
file_table_t *new_fileTable() {

  file_table_t *t = safeMalloc(sizeof(file_table_t));

  t->numberOfFilesOpen = 0;
  t->table = safeMalloc(MAX_FILES_OPEN * sizeof(file_open_t *));

  for (int i = 0; i < MAX_FILES_OPEN; i++)
    t->table[i] = NULL;

  return t;
}

/* Frees ft's alloced memory. */
void free_fileTable(file_table_t *ft) {
  for (int i = 0; i < MAX_FILES_OPEN; i++)
    if (ft->table[i])
      (ft->numberOfFilesOpen)--;

  free(ft->table);
  free(ft);
}

/* Returns the name of the file with the given fd if it exists and NULL
   otherwise. */
char *getFileName(file_table_t *ft, int fd) {
  return ft->table[fd] ? ft->table[fd]->fileName : NULL;
}

/* Returns 1 if name is stored in fd and 0 otherwise. */
int containsFile(file_table_t *ft, char *name) {
  for (int i = 0; i < MAX_FILES_OPEN; i++)
    if (ft->table[i] && !strcmp(ft->table[i]->fileName, name))
      return 1;
  return 0;
}

/* Adds a file whose attributes are specified as arguments to the table ft. */
int addFile(file_table_t *ft, permission mode, char *name) {
  if (ft->numberOfFilesOpen >= MAX_FILES_OPEN)
    return TECNICOFS_ERROR_MAXED_OPEN_FILES;

  if (containsFile(ft, name))
    return TECNICOFS_ERROR_FILE_IS_OPEN;

  if (lookup(fs, name) == UNEXISTENT)
    return TECNICOFS_ERROR_FILE_NOT_FOUND;

  file_open_t *f = safeMalloc(sizeof(file_open_t));
  f->fileName = safeMalloc(sizeof(char) * (strlen(name) + 1));
  strcpy(f->fileName, name);
  f->mode = mode;

  for (int fd = 0; fd < MAX_FILES_OPEN; fd++)
    if (ft->table[fd] == NULL) {
      (ft->numberOfFilesOpen)++;
      ft->table[fd] = f;
      return fd;
    }

  return TECNICOFS_ERROR_OTHER;
}

/* Removes the file with the given fd of the table ft. */
int removeFile(file_table_t *ft, int fd) {
  if (ft->numberOfFilesOpen == 0)
    return TECNICOFS_ERROR_FILE_NOT_OPEN;

  (ft->numberOfFilesOpen)--;
  free(ft->table[fd]->fileName);
  free(ft->table[fd]);
  ft->table[fd] = NULL;
  return EXIT_SUCCESS;
}

/* Creates the server. */
int initializeSocket(char *address) {
  struct sockaddr_un serv_addr;
  int serverlen;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0); // Creates socket.

  if (sockfd < 0)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  if (unlink(address) && errno != ENOENT){
    printf("Fatal unlink error. Terminating\n");
    exit(EXIT_FAILURE);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, address);

  serverlen =
      sizeof(char) * strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, serverlen) < 0)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  if (listen(sockfd, MAX_CONN_SRV) < 0)
    return TECNICOFS_ERROR_OTHER;

  return EXIT_SUCCESS;
}

/* Returns 0 if the file with the given if is opened with the given mode and a
   code error otherwise. */
int fileOpenMode(file_table_t *ft, int fd, permission mode) {
  if (ft->table[fd] == NULL) // If it was opened then it must be on the table.
    return TECNICOFS_ERROR_FILE_NOT_OPEN;

  else if (!(ft->table[fd]->mode == mode || ft->table[fd]->mode == RW))
    return TECNICOFS_ERROR_INVALID_MODE;

  return EXIT_SUCCESS;
}

/* Processes the input from the client. */
void *handleRequest(void *arg) {
  int newsocketfd = *((int *)arg), numTokens;
  char token;
  char line[MAX_INPUT_SIZE], name[MAX_INPUT_SIZE];
  char arg1[MAX_INPUT_SIZE], arg2[MAX_INPUT_SIZE];
  char buffer[MAX_INPUT_SIZE];
  int numBytesRead, answer, fd, mode;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  // Client's table of open files.
  file_table_t *fileTable = new_fileTable(newsocketfd);

  // Obtains socket's owner.
  struct ucred ucred;
  int len = sizeof(ucred);
  if (getsockopt(newsocketfd, SOL_SOCKET, SO_PEERCRED, &ucred,
                 (socklen_t *)&len) < 0)
    return NULL;

  while ((numBytesRead = read(newsocketfd, line, MAX_INPUT_SIZE)) > 0) {
    numTokens = sscanf(line, "%c %s %[^$]", &token, arg1, arg2);
    switch (token) {
    // Deletes file.
    case 'd': // arg1: name
      if (numTokens != 2)
        errorParse();
      answer = delete (fs, arg1, ucred.uid);
      break;
    // Renames file.
    case 'r': // arg1: current file name, arg2: new file name
      if (numTokens != 3)
        errorParse();
      answer = editFileName(fs, arg1, arg2, ucred.uid);
      break;
    // Creates file.
    case 'c': // arg1: file name, arg2: permissions (<owner><others>)
      if (numTokens != 3)
        errorParse();
      answer = create(fs, arg1, ucred.uid, arg2[0] - '0', arg2[1] - '0');
      break;
    // Opens file and adds it to the table.
    case 'o': // arg1: name of the file, arg2: mode
      if (numTokens != 3)
        errorParse();
      strcpy(name, arg1);
      mode = atoi(arg2);
      if ((answer = addFile(fileTable, mode, name)) >= 0)
        answer = openFile(fs, name, mode, ucred.uid);
      break;
    // Closes the file and removes it from the table.
    case 'x': // arg1: fd
      if (numTokens != 2)
        errorParse();
      fd = atoi(arg1);
      if ((answer = closeFile(fs, getFileName(fileTable, fd))) >= 0)
        answer = removeFile(fileTable, fd);
      break;
    // Reads from the file.
    case 'l': // arg1: fd, arg2: len
      if (numTokens != 3)
        errorParse();
      fd = atoi(arg1);
      if ((answer = fileOpenMode(fileTable, fd, READ)) == EXIT_SUCCESS)
        answer = readFile(fs, getFileName(fileTable, fd), atoi(arg2), buffer);
      break;
    // Writes in the file.
    case 'w': // arg1: fd, arg2: data to write in the file
      if (numTokens != 3)
        errorParse();
      fd = atoi(arg1);
      if ((answer = fileOpenMode(fileTable, fd, WRITE)) == EXIT_SUCCESS)
        answer = fileWrite(fs, getFileName(fileTable, fd), arg2);
      break;
    default: // Error.
      errorParse();
    }
    write(newsocketfd, (void *)&answer, sizeof(int));

    if (token == 'l' && answer >= 0)
      write(newsocketfd, buffer, sizeof(char) * (strlen(buffer) + 1));
  }
  free_fileTable(fileTable);
  close(newsocketfd);
  return NULL;
}

/* Handles Ctrl+C to terminate server */
void terminate_server(int s) {
  (void)s; // Makes gcc happy
  close(sockfd);
}

int main(int argc, char *const argv[]) {
  parseArgs(argc, argv);
  signal(SIGINT, terminate_server);
  int threadMax = 0;
  int numThreads = INITIAL_THREADS;
  pthread_t *tid = safeMalloc(sizeof(pthread_t) * INITIAL_THREADS);
  FILE *output = openOutputFile();
  struct timeval start, end;
  int newsockfd, clientlen;
  struct sockaddr_un cli_addr;

  fs = new_tecnicofs();
  initializeSocket(socketName);

  checkError(gettimeofday(&start, NULL), TIME_ERROR);
  for (;;) {
    clientlen = sizeof(cli_addr);
    newsockfd =
        accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clientlen);

    if (newsockfd < 0 && errno == EBADF)
      break;

    else if (newsockfd < 0)
      exit(EXIT_FAILURE);

    if (threadMax == numThreads) {
      numThreads += INITIAL_THREADS;
      tid = realloc(tid, sizeof(pthread_t) * numThreads);
    }
    SAFE_PTHREAD(pthread_create(&tid[threadMax++], 0, handleRequest,
                                (void *)&newsockfd));
  }

  checkError(gettimeofday(&end, NULL), TIME_ERROR);

  for (int i = 0; i < threadMax; ++i)
    SAFE_PTHREAD(pthread_join(tid[i], NULL));

  print_tecnicofs_tree(output, fs);
  fprintf(stdout, "TecnicoFS completed in %.4f seconds.\n",
          timeDiff(start, end));
  checkError(fclose(output), "Error on closing file.\n");
  free_tecnicofs(fs);
  free(tid);
  exit(EXIT_SUCCESS);
}
