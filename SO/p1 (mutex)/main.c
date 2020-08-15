#include "fs.h"
#include "multithreading.h"
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAX_COMMANDS 150000
#define MAX_INPUT_SIZE 100

/* Command-access and iNumber latch. */
DECLARE_LOCK(commandLock);
/* tecnicofs operations latch */
DECLARE_LOCK(fsLock);

int numberThreads = 0;
tecnicofs* fs;

/* Array of strings to store commands before executing them.
 * Works like a queue but limited to 150k commands. */
char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];

/* Index number of next command to insert to inputCommands. */
int numberCommands = 0;

/* Posision of first command to be removed from inputCommands / processed. */
int headQueue = 0;

/* Displays a simple message to user */
static void displayUsage(const char* appName)
{
    printf("Usage: %s inputFile.txt outputFile.txt numOfThreads \n", appName);
    exit(EXIT_FAILURE);
}

/* Verifies if it has received correct number of arguments. */
static void parseArgs(long argc, char* const argv[])
{
    if (argc != 4 || atoi(argv[3]) < 1) {
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }
}

/* Copies *data to inputCommands unused free array position.
 * Returns 1 on success and 0 on failure (more than 150k commands). */
int insertCommand(char* data)
{
    if (numberCommands != MAX_COMMANDS) {
        strcpy(inputCommands[numberCommands++], data);
        return 1;
    }
    return 0;
}

/* Removes oldest command inserted in inputCommands.
 * Returns pointer to command "removed".
 * Decreases numberOfCommands and headQueue. */
char* removeCommand()
{
    if (numberCommands > 0) {
        numberCommands--;
        return inputCommands[headQueue++];
    }
    return NULL;
}

/* Outputs simple error message to stderr. */
void errorParse()
{
    fprintf(stderr, "Error: command invalid\n");
}

/* Copies commands from stdin to inputCommands queue (array) using
 * insertCommand, outputs error messages to stderr on bad commands
 * (invalid syntax). MUST NOT BE RUN IN MULTIPLE THREADS. */
void processInput(FILE* input)
{
    char line[MAX_INPUT_SIZE];

    while (fgets(line, sizeof(line) / sizeof(char), input)) {
        char token;
        char name[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s", &token, name);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
        case 'c':
        case 'l':
        case 'd':
            if (numTokens != 2)
                errorParse();
            if (insertCommand(line))
                break;
            return;
        case '#':
            break;
        default: { /* error */
            errorParse();
        }
        }
    }
}

/* Removes commands from inputCommands array and applies them to tecnicoFS. */
void applyCommands()
{
    while (numberCommands > 0) {
        WR_LOCK(commandLock);
        const char* command = removeCommand();
        if (command == NULL) {
            continue;
        }

        char token;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s", &token, name);
        if (numTokens != 2) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
        int iNumber;
        switch (token) {
        case 'c': /* Creates a file - adds it to tecnicoFS. */
            iNumber = obtainNewInumber(fs);
            UNLOCK(commandLock);
            WR_LOCK(fsLock);
            create(fs, name, iNumber);
            UNLOCK(fsLock);
            break;
        case 'l': /* Prints the file's iNumber if found. */
            UNLOCK(commandLock);
            RD_LOCK(fsLock);
            searchResult = lookup(fs, name);
            UNLOCK(fsLock);
            if (!searchResult)
                printf("%s not found\n", name);
            else
                printf("%s found with inumber %d\n", name, searchResult);
            break;
        case 'd': /* Deletes file. */
            UNLOCK(commandLock);
            WR_LOCK(fsLock);
            delete (fs, name);
            UNLOCK(fsLock);
            break;
        default: { /* Error. */
            UNLOCK(commandLock);
            fprintf(stderr, "Error: command to apply\n");
            exit(EXIT_FAILURE);
        }
        }
    }
}

int main(int argc, char* argv[])
{
    struct timeval start, end;

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    CHECK_ERROR(!input || !output, "File I/O error. Terminating program.\n");
    parseArgs(argc, argv);

    /* Inicializes tecnicoFS. */
    fs = new_tecnicofs();
    /* Processes input sequentially, loading it to a vector. */
    processInput(input);

    INIT_LOCK(commandLock);
    INIT_LOCK(fsLock);

    CHECK_ERROR(gettimeofday(&start, NULL), "Fatal error on gettimeofday.\n");

    numberThreads = atoi(argv[3]);
    threadPool(numberThreads, (void*)applyCommands);

    print_tecnicofs_tree(output, fs); /* Prints final state of tecnicoFS. */

    CHECK_ERROR(fclose(input) || fclose(output), "Fatal file-closing error.\n");

    free_tecnicofs(fs);

    DESTROY_LOCK(commandLock);
    DESTROY_LOCK(fsLock);

    CHECK_ERROR(gettimeofday(&end, NULL), "Fatal error on gettimeofday.\n");
    float time_diff = (end.tv_sec - start.tv_sec)
        + (end.tv_usec - start.tv_usec) / 1e6;

    printf("TecnicoFS completed in %.4f seconds.\n", time_diff);

    exit(EXIT_SUCCESS);
}
