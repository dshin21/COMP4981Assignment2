#include "client.h"

pthread_mutex_t mutex;

int clnt(const int qid, const int priority) {
    pthread_t controlThread;
    struct params p;
    int pid;
    int running = 1;
    struct msgbuf mBuffer;

    pid = (int) getpid();

    // Start thread to check if program should stop running
    p.pRunning = &running;
    p.qid = qid;
    p.pid = pid;
    p.priority = priority;

    if (pthread_create(&controlThread, NULL, client_control, (void *) &p)) {
        perror("Could not start thread");
        return 1;
    }

    // If the message is not full that means it is the last one
    while (running) {
        memset(&mBuffer, 0, sizeof(struct msgbuf));
        if (read_message_blocking(qid, pid, &mBuffer) <= 0) {
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

void *client_control(void *params) {
    char line[MSGSIZE];
    char command[MSGSIZE];

    struct params *p = params;
    int *pRunning = p->pRunning;
    int priority = p->priority;
    int pid = p->pid;
    int qid = p->qid;

    struct msgbuf buffer;

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
                } else {

                    // Place the filename and child PID into buffer
                    memset(&buffer, 0, sizeof(struct msgbuf));
                    buffer.mtype = C_TO_S;
                    sprintf(buffer.mtext, "%d/%d\t%s", pid, priority, command);
                    buffer.mlen = (int) strlen(buffer.mtext);

                    // Send the buffer
                    if (send_message(qid, &buffer) == -1) {
                        perror("Problem writing to the message queue");
                    }
                }
            }
        }
    }
    return NULL;
}
