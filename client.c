#include "client.h"

pthread_mutex_t mutex;

int client(const int server_qid, const int client_priority, const char *client_file_name) {
    pthread_t controlThread;
    int pid;
    int running = 1;
    struct params p;
    struct msgbuf mBuffer;

    // Start thread to check if program should stop running
    p.pRunning = &running;
    p.qid = server_qid;
    p.pid = (int) getpid();
    p.priority = client_priority;

    if (pthread_create(&controlThread, NULL, client_control, (void *) &p)) {
        perror("Could not start thread");
        return 1;
    }

    client_send_info(&p, &mBuffer, p.pid, client_file_name);

    // If the message is not full that means it is the last one
    while (running) {
        memset(&mBuffer, 0, sizeof(struct msgbuf));
        if (read_message_blocking(server_qid, p.pid, &mBuffer) <= 0) {
            sched_yield();
            continue;
        }

        // Check if the server returned an error opening the file
        if (!strcmp(mBuffer.mtext, "Error: Could not open file")) {
            // If it did, print the error and return
            printf("%s\n", mBuffer.mtext);
            fflush(stdout);
        } else {
            // Otherwise, print the first part of the file
            printf("%s", mBuffer.mtext);
            fflush(stdout);
        }
    }

    // Print the last message
    printf("%s", mBuffer.mtext);
    fflush(stdout);

    pthread_join(controlThread, 0);
    return 0;
}

void client_send_info(struct params *p, struct msgbuf *mBuffer, int pid, const char *client_file_name) {
    memset(mBuffer, 0, sizeof(struct msgbuf));
    mBuffer->mtype = CLIENT_TO_SERVER;
    sprintf(mBuffer->mtext, "%d %d %s", pid, p->priority, client_file_name);
    mBuffer->mlen = (int) strlen(mBuffer->mtext);

    if (send_message(p->qid, mBuffer) == -1) {
        perror("Error: writing to message queue");

    }
}

void *client_control(void *params) {
    char line[MSGSIZE];
    char command[MSGSIZE];
    struct params *p = params;
    int *pRunning = p->pRunning;

    while (*pRunning) {
        if (fgets(line, MSGSIZE, stdin)) {
            if (sscanf(line, "%s", command) == 1) {
                // If it is the quit command
                if (!strcmp(command, "quit") ||
                    !strcmp(command, "stop") ||
                    !strcmp(command, "q") ||
                    !strcmp(command, "s")) {
                    pthread_mutex_lock(&mutex);
                    *pRunning = 0;
                    pthread_mutex_unlock(&mutex);
                    kill(0, SIGINT);
                }
            }
        }
    }
    return NULL;
}
