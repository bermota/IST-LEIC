#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include "fs.h"
#include "constants.h"
#include "sync.h"

char* global_inputFile = NULL;
char* global_outputFile = NULL;
int numberThreads = 0;
extern int numberTrees;

LOCK_T commandsLock;
tecnicofs* fs;

char inputCommands[BUFFER_SIZE][MAX_INPUT_SIZE];

/* Circular queue - position counters */
int prodptr = 0;
int consptr = 0;

/* Circular queue - semaphores */
sem_t mayInsert;
sem_t mayRemove;

static void displayUsage (const char* appName){
    printf("Usage: %s input_filepath output_filepath threads_number\n", appName);
    exit(EXIT_FAILURE);
}

/* Verifies if all arguments exist and are correct. */
static void parseArgs (long argc, char* const argv[]){
    if (argc != NUMBER_ARGUMENTS) {
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }

    global_inputFile = argv[1];
    global_outputFile = argv[2];
    numberThreads = atoi(argv[3]);
    numberTrees = atoi(argv[4]);

    if (numberThreads < 1 || numberTrees < 1) {
        fprintf(stderr, "Invalid number of threads or trees.\n");
        displayUsage(argv[0]);
    }
}

/* Opens the output file and returns it. */
FILE * openOutputFile() {
    FILE *fp = fopen(global_outputFile, "w");
    checkError(!fp, "Error opening output file");
    return fp;
}


/* Returns the time difference (in seconds) between the given arguments. */
float timeDiff(struct timeval start, struct timeval end){
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}

/* Prints the file's name with its iNumber if found. */
void printFileName(char *name, int searchResult){
    if(!searchResult)
        printf("%s not found\n", name);
    else
        printf("%s found with inumber %d\n", name, searchResult);
}

/* Displays an error message indicating the line where it occured. */
void errorParse(int lineNumber){
    fprintf(stderr, "Error: line %d invalid\n", lineNumber);
    exit(EXIT_FAILURE);
}

/* Inserts data in the circular queue (produce). */
int insertCommand(char* data) {
    WAIT(&mayInsert);
    strcpy(inputCommands[prodptr], data);
    prodptr = (prodptr + 1) % BUFFER_SIZE;
    POST(&mayRemove);
    return 1;
}

/* Removes data from the circular queue (consume). */
char* removeCommand() {
    char *command = inputCommands[consptr];
    consptr = (consptr + 1) % BUFFER_SIZE;
    return command;
}

/* Processes the input file (producer). */
void* processInput(){
    char token;
    char name[MAX_INPUT_SIZE];
    char newName[MAX_INPUT_SIZE];
    char line[MAX_INPUT_SIZE];
    int lineNumber = 0;
    int numTokens;

    FILE* inputFile = fopen(global_inputFile, "r");
    checkError(!inputFile, "Fatal I/0 error.\n");
    

    while (fgets(line, sizeof(line)/sizeof(char), inputFile)){
        lineNumber++;

        numTokens = sscanf(line, "%c %s %s", &token, name, newName);
        // Perform minimal validation.
        if (numTokens < 1)
            continue;
            
        switch (token) {
            case 'c':
            case 'l':
            case 'd':
                if(numTokens != 2)
                    errorParse(lineNumber);
                if(insertCommand(line)){
                    break;
                }
                return NULL;
            case 'r':
                if(numTokens != 3)
                    errorParse(lineNumber);
                if (insertCommand(line)){
                    break;
                }
                return NULL;
            case '#':
                break;
            default: // Error.
                errorParse(lineNumber);
        }
    }
    // Fills inputCommands with the final command.
    for (int i = 0; i < numberThreads ; i++)
        insertCommand(FINAL_COMMAND);

    checkError(fclose(inputFile), "Fatal file-closing error.\n");
    return NULL;
}

/* Processes the circular queue until it's meant to be stopped (consumer). */
void* applyCommands(){
    char token;
    char name[MAX_INPUT_SIZE];
    char newName[MAX_INPUT_SIZE];
    char *command;
    int iNumber;

    while (1){
        WAIT(&mayRemove);
        
        WR_LOCK(&commandsLock);

        command = removeCommand();

        if (!strcmp(command, FINAL_COMMAND)){
            UNLOCK(&commandsLock);
            POST(&mayInsert);  
            break;
        }

        sscanf(command, "%c %s %s", &token, name, newName);

        if (token == 'c')
            iNumber = obtainNewInumber(fs);

        UNLOCK(&commandsLock);
        POST(&mayInsert);

        switch (token) {
            case 'c':
                create(fs, name, iNumber);
                break;
            case 'l':
                printFileName(name, lookup(fs, name));
                break;
            case 'd':
                delete(fs, name);
                break;
            case 'r':
                editFileName(fs, name, newName);
                break;
            default: // Error.
                checkError(1, "Error: commands to apply\n");
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    parseArgs(argc, argv);
    pthread_t inputThread;
    struct timeval start, end;
    FILE * output = openOutputFile();

    fs = new_tecnicofs();

    INIT_LOCK(&commandsLock);
    SEM_INIT(&mayRemove, 0, 0);
    SEM_INIT(&mayInsert, 0, BUFFER_SIZE);

    checkError(gettimeofday(&start, NULL), TIME_ERROR);

    checkError(pthread_create(&inputThread, NULL, processInput, NULL), "Error.\n");
    runThreads(numberThreads, applyCommands);
    checkError(pthread_join(inputThread, NULL), "Error.\n");

    checkError(gettimeofday(&end, NULL), TIME_ERROR);

    print_tecnicofs_tree(output, fs);

    SEM_DESTROY(&mayRemove);
    SEM_DESTROY(&mayInsert);
    DESTROY_LOCK(&commandsLock);

    fflush(output);
    checkError(fclose(output), "Error closing file.\n");
    
    fprintf(stdout, "TecnicoFS completed in %.4f seconds.\n", timeDiff(start, end));
    free_tecnicofs(fs);
    exit(EXIT_SUCCESS);
}
