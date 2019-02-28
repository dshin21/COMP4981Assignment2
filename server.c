#include "server.h"
#include <errno.h>

static int serverQID;
static int globalSID;

int srvr() {
    // Threading variable
    pthread_t controlThread;
    int running = 1;

    // Client
    int pid;
    int priority;
    char filename[MSGSIZE];
    FILE *file;

    int i;
    int res;
    int returnCode = 0;
    int pleaseQuit = 0;
    struct msgbuf sendBuffer;

    // Exit signal
    signal(SIGINT, catchSig);

    // Start thread to check if program should stop running
    if (pthread_create(&controlThread, NULL, server_control, (void *) &running)) {
        perror("Could not start thread");
        return 1;
    }

    // Open queue
    if ((serverQID = open_queue((int) getpid())) == -1) {
        perror("Could not open queue");
        return 1;
    } else {
//        fprintf(stdout, "Use './assign2 [high|normal|low] %d' to make a request to this server\n", serverQID);
        printf("\nCommand for client: ./MessageQueueApp client [S|M|L] <filename>\n");
        fflush(stdout);
    }

    if ((globalSID = create_semaphore((int) getpid())) < 0) {
        perror("Could not create semaphore");
        return 1;
    }

    V(globalSID);

    while (running) {
        if (!acceptClients(&pid, &priority, filename)) {
            sched_yield();
            continue;
        }

        // Fork and serve if it is the child
        if (!fork()) {
            file = fopen(filename, "r");

            if (file == NULL) {
                sendBuffer.mtype = pid;
                strcpy(sendBuffer.mtext, "Error: Could not open file");
                sendBuffer.mlen = 27;

                if (send_message(serverQID, &sendBuffer) == -1) {
                    perror("Problem sending to client");
                }

                perror("Could not open file");
                return 0;
            }

            printf("%d> child started\n", getpid());
            sendBuffer.mtype = pid;

            while (!feof(file)) {
                P(globalSID);
                for (i = 0; i < priority; i++) {
                    res = (int) read_file(file, &sendBuffer);

                    if (res == 0) {
                        returnCode = 0;
                        pleaseQuit = 1;
                    }

                    if (res < 0) {
                        perror("Problem reading from file");
                        returnCode = pleaseQuit = 1;
                    }

                    if (send_message(serverQID, &sendBuffer) == -1) {
                        perror("Problem sending message");
                        returnCode = pleaseQuit = 1;
                    }
                }
                V(globalSID);

                sched_yield();

                if (pleaseQuit) {
                    break;
                }
            }

            printf("%d> child is finished and exiting\n", getpid());
            return returnCode;
        }
    }

    if (remove_semaphore(globalSID) == -1) {
        perror("Could not remove semaphore");
    }

    // Close queue when parent exits
    if (remove_queue(serverQID) == -1) {
        perror("Problem with closing the queue");
        return (1);
    }

    pthread_join(controlThread, 0);
    return 0;
}

void *server_control(void *params) {
    char line[256];
    char command[256];
    int *pRunning = (int *) params;

    while (*pRunning) {
        if (fgets(line, 256, stdin)) {
            if (sscanf(line, "%s", command) == 1) {
                if (!strcmp(command, "quit") ||
                    !strcmp(command, "stop") ||
                    !strcmp(command, "q") ||
                    !strcmp(command, "s")) {
                    kill(0, SIGINT);
                    *pRunning = 0;
                }
            }
        }
        sched_yield();
    }
    return NULL;
}

void catchSig(int sig) {
    remove_semaphore(globalSID);
    remove_queue(serverQID);
    exit(0);
}

int acceptClients(int *pPid, int *pPriority, char *filename) {
    struct msgbuf buffer;
    memset(&buffer, 0, sizeof(struct msgbuf));

    // If a new client is found...
    if (read_message_blocking(serverQID, C_TO_S, &buffer) > 0) {
        printf("server> New request: [%s]\n", buffer.mtext);

        // Grab the filename and pid
        memset(filename, 0, MSGSIZE);
        parseClientRequest(buffer.mtext, pPid, pPriority, filename);
        return 1;
    }
    return 0;
}

void parseClientRequest(const char *message, int *pid, int *priority, char *filename) {
    int i;
    char tmp[MSGSIZE];
    char *fileStart = NULL;
    char *pidStart = NULL;
    char *priorityStart = NULL;

    strcpy(tmp, message);
    pidStart = tmp;

    for (i = 0; tmp[i]; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            priorityStart = tmp + i + 1;
        }

        if (tmp[i] == '\t') {
            tmp[i] = '\0';
            fileStart = tmp + i + 1;
        }
    }
    *pid = atoi(pidStart);
    *priority = atoi(priorityStart);
    memcpy(filename, fileStart, strlen(fileStart));
}