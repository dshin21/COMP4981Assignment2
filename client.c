#include "client.h"

int client(const int s_qid, const int c_priority, const char *c_file_name) {
    pthread_t thread_exit_watcher;
    struct client_info c_info;
    struct message_object msg_obj;

    c_info.client_thread_status = 1;
    c_info.client_qid = s_qid;
    c_info.client_pid = (int) getpid();
    c_info.client_priority = c_priority;

    if (pthread_create(&thread_exit_watcher, NULL, client_exit_handler, (void *) &c_info)) {
        perror("Error: creating thread");
        return 1;
    }

    client_send_info(&c_info, &msg_obj, c_info.client_pid, c_file_name);

    while (c_info.client_thread_status) {
        memset(&msg_obj, 0, sizeof(struct message_object));
        if (read_message(s_qid, c_info.client_pid, &msg_obj, BLOCKING) <= 0) {
            sched_yield();
            continue;
        }

        if (!strcmp(msg_obj.mtext, ERROR_FILE)) {
            printf("Error: Reading file\n");
            fflush(stdout);
        } else {
            printf("%s", msg_obj.mtext);
            fflush(stdout);
        }
    }

    printf("%s", msg_obj.mtext);
    fflush(stdout);

    pthread_join(thread_exit_watcher, 0);
    return 0;
}

void client_send_info(struct client_info *c_info, struct message_object *mBuffer, int c_pid, const char *c_file_name) {
    memset(mBuffer, 0, sizeof(struct message_object));
    sprintf(mBuffer->mtext, "%d %d %s", c_pid, c_info->client_priority, c_file_name);
    mBuffer->mlen = (int) strlen(mBuffer->mtext);
    mBuffer->mtype = CLIENT_TO_SERVER;

    if (send_message(c_info->client_qid, mBuffer) == -1) perror("Error: Writing to message queue");
}

void *client_exit_handler(void *c_info) {
    char temp[128];
    char user_input[128];

    struct client_info *p = c_info;
    int *is_running = &p->client_thread_status;

    while (*is_running) {
        if (fgets(temp, 128, stdin) && sscanf(temp, "%s", user_input) == 1 && !strcmp(user_input, "s")) {
            kill(0, SIGINT);
            *is_running = 0;
        }
        sched_yield();
    }
    return NULL;
}
