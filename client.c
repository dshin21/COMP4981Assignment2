#include "client.h"

pthread_mutex_t mutex;

int client(const int server_qid, const int client_priority, const char *client_file_name) {
    pthread_t controlThread;
    int thread_status = 1;
    struct client_info c_info;
    struct message_object msg_obj;

    // Start thread to check if program should stop running
    c_info.client_thread_status = &thread_status;
    c_info.client_qid = server_qid;
    c_info.client_pid = (int) getpid();
    c_info.client_priority = client_priority;

    if (pthread_create(&controlThread, NULL, client_control, (void *) &c_info)) {
        perror("Could not start thread");
        return 1;
    }

    client_send_info(&c_info, &msg_obj, c_info.client_pid, client_file_name);

    // If the message is not full that means it is the last one
    while (thread_status) {
        memset(&msg_obj, 0, sizeof(struct message_object));
        if (read_message(server_qid, c_info.client_pid, &msg_obj, BLOCKING) <= 0) {
            sched_yield();
            continue;
        }

        // Check if the server returned an error opening the file
        if (!strcmp(msg_obj.mtext, "Error: Could not open file")) {
            // If it did, print the error and return
            printf("%s\n", msg_obj.mtext);
            fflush(stdout);
        } else {
            // Otherwise, print the first part of the file
            printf("%s", msg_obj.mtext);
            fflush(stdout);
        }
    }

    // Print the last message
    printf("%s", msg_obj.mtext);
    fflush(stdout);

    pthread_join(controlThread, 0);
    return 0;
}

void client_send_info(struct client_info *p, struct message_object *mBuffer, int pid, const char *client_file_name) {
    memset(mBuffer, 0, sizeof(struct message_object));
    mBuffer->mtype = CLIENT_TO_SERVER;
    sprintf(mBuffer->mtext, "%d %d %s", pid, p->client_priority, client_file_name);
    mBuffer->mlen = (int) strlen(mBuffer->mtext);

    if (send_message(p->client_qid, mBuffer) == -1) {
        perror("Error: writing to message queue");
    }
}

void *client_control(void *params) {
    char line[MSGSIZE];
    char command[MSGSIZE];
    struct client_info *p = params;
    int *pRunning = p->client_thread_status;

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
