#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../constants.h"
#include "tecnicofs-client-api.h"

int sockfd = UNEXISTENT;

/* Sends command to the server. */
int sendToServer(char *command) {
  if (write(sockfd, command, (strlen(command) + 1) * sizeof(char)) < 0)
    return TECNICOFS_ERROR_OTHER;
  return EXIT_SUCCESS;
}

/* Reads the server's answer to the (previously) sent command and
 * returns it. */
int answerFromServer() {
  int value;
  if (read(sockfd, (void *)&value, sizeof(int)) < 0)
    return TECNICOFS_ERROR_CONNECTION_ERROR;
  return (int)value;
}

/* Creates and connects to address, returning EXIT_SUCCESS if the operation was
 * successful and an error otherwise. */
int tfsMount(char *address) {
  struct sockaddr_un serv_addr;
  int serverlen;

  if (sockfd != UNEXISTENT)
    return TECNICOFS_ERROR_OPEN_SESSION;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sockfd < 0)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, address);

  serverlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, serverlen) < 0)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  return EXIT_SUCCESS;
}

/* Closes the client. */
int tfsUnmount() {

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  return close(sockfd);
}

/* Creates a file with the given filename and its permissions.
 * Returns 0 or an error code. */
int tfsCreate(char *filename, permission ownerPermissions,
              permission othersPermissions) {
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "c %s %d%d", filename,
           ownerPermissions, othersPermissions);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}

/* Deletes the file with the given filename. Returns 0 or an error code. */
int tfsDelete(char *fileName) {
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "d %s", fileName);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}

/* Renames the file named fileNameOld to fileNameNew. Returns 0 or an error
   code. */
int tfsRename(char *fileNameOld, char *fileNameNew) {
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "r %s %s", fileNameOld,
           fileNameNew);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}

/* Opens the file with the given filename to the given mode. Returns 0 or an
   error code. */
int tfsOpen(char *filename, permission mode) {
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  if (!filename || !strlen(filename))
    return EXIT_FAILURE;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "o %s %d", filename, mode);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}

/* Closes the file with the given fd. Returns or an error code. */
int tfsClose(int fd) {
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "x %d", fd);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}

/* Reads from the file with the given fd to buffer to a maximum of len chars.
   Returns the number of read characters or an error code. */
int tfsRead(int fd, char *buffer, int len) {
  char formattedCommand[MAX_INPUT_SIZE];
  int answer;
  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  snprintf(formattedCommand, MAX_INPUT_SIZE, "l %d %d", fd, len);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  answer = answerFromServer();

  if (answer < 0) // If error.
    return answer;

  read(sockfd, buffer, answer + 1);

  return answer;
}

/* Writes buffer into the file with the given fd to a maximum of len chars.
   Returns 0 or an error code. */
int tfsWrite(int fd, char *buffer, int len) {

  int len_buffer = strlen(buffer);
  int min = (len_buffer < len ? len_buffer : len);
  char command[10];
  char formattedCommand[MAX_INPUT_SIZE];

  if (sockfd == UNEXISTENT)
    return TECNICOFS_ERROR_NO_OPEN_SESSION;

  sprintf(command, "w %d", fd);
  snprintf(formattedCommand, min + strlen(command) + 2, "%s %s", command,
           buffer);

  if (sendToServer(formattedCommand) != EXIT_SUCCESS)
    return TECNICOFS_ERROR_CONNECTION_ERROR;

  return answerFromServer();
}
