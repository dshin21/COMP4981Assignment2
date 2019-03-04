#include "server.h"

int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info);

int server_entry() {
    // Threading variable
    pthread_t thread_exit_watcher;
    int exit_watcher = 1;

    // Client Info
//    int i;
    int main_return_val = 0;

    struct message_object s_buffer;
    struct client_info c_info;
    memset(&c_info, 0, sizeof(struct client_info));

    // Exit signal
    signal(SIGINT, abort_cleanup);

    // Start thread to check if program should stop running
    if (pthread_create(&thread_exit_watcher, NULL, exit_handler, &exit_watcher)) {
        perror("Error: creating thread");
        return 1;
    }

    // Open queue
    if ((server_qid = queue_open((int) getpid())) == -1) {
        perror("Error: opening message queue");
        return 1;
    } else {
        printf("\nCommand for client: ./MessageQueueApp client %d [S|M|L] <filename>\n", server_qid);
        fflush(stdout);
    }

    // create semaphore
    if ((semaphore_id = create_semaphore((int) getpid())) < 0) {
        perror("Error: creating semaphore");
        return 1;
    }

    V(semaphore_id);

    while (exit_watcher) {
        if (!acceptClients(&c_info)) {
            sched_yield();
            continue;
        }

        // Fork and serve if it is the child
        if (!fork()) {
            printf("forked %d\n", getpid());
            c_info.client_ptr_file = open_file(c_info.client_file_name, "r");

            if (c_info.client_ptr_file == NULL) {
                server_send_error_msg_to_client(&s_buffer, &c_info);
                return 0;
            }

            printf("child started PID: %d\n", getpid());
            return server_send_file_to_client(&s_buffer, &c_info);;
        }
    }

    if (remove_semaphore(semaphore_id) == -1) {
        perror("Could not remove semaphore");
    }

    // Close queue when parent exits
    if (remove_queue(server_qid) == -1) {
        perror("Problem with closing the queue");
        return (1);
    }

    pthread_join(thread_exit_watcher, 0);
    return 0;
}

int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info) {
    int return_val = 0;
    int i;
    s_buffer->mtype = c_info->client_pid;

    while (!feof((*c_info).client_ptr_file)) {
        P(semaphore_id);
        for (i = 0; i < (*c_info).client_priority; i++) {
            (*c_info).client_file_size = (int) read_file((*c_info).client_ptr_file, s_buffer);

            if ((*c_info).client_file_size == 0) {
                return_val = 0;
            }

            if ((*c_info).client_file_size < 0) {
                perror("Error: Reading from file");
                return_val = 1;
            }

            if (send_message(server_qid, s_buffer) == -1) {
                perror("Error: Sending message");
                return_val = 1;
            }
        }

        V(semaphore_id);

        sched_yield();

        if (return_val) {
            break;
        }
    }
    printf("%d> child is finished and exiting\n", getpid());
    return return_val;
}

void server_send_error_msg_to_client(struct message_object *s_buffer, struct client_info *c_info) {
    s_buffer->mtype = c_info->client_pid;
    strcpy(s_buffer->mtext, ERROR_FILE);
    s_buffer->mlen = (int) strlen(ERROR_FILE);

    if (send_message(server_qid, s_buffer) == -1) {
        perror("Error: could not send to client");
    }

    printf("Error: reading file\n");
}

void *exit_handler(void *exit_watcher) {
    int *pRunning = (int *) exit_watcher;
    char temp[128];
    char user_input[128];

    while (*pRunning) {
        if (fgets(temp, 128, stdin) && sscanf(temp, "%s", user_input) == 1 && !strcmp(user_input, "s")) {
            kill(0, SIGINT);
            *pRunning = 0;
        }
        sched_yield();
    }
    return NULL;
}

void abort_cleanup(int code) {
    remove_semaphore(semaphore_id);
    remove_queue(server_qid);
    exit(0);
}

int acceptClients(struct client_info *c_info) {
    struct message_object buffer;
    memset(&buffer, 0, sizeof(struct message_object));

    // If a new client is found...
    if (read_message(server_qid, CLIENT_TO_SERVER, &buffer, BLOCKING) > 0) {
        printf("New Client - %s\n", buffer.mtext);

        // Grab the filename and pid
        parseClientRequest(buffer.mtext, &c_info->client_pid, &c_info->client_priority, c_info->client_file_name);
        return 1;
    }
    return 0;
}

void parseClientRequest(const char *message, int *pid, int *priority, char *filename) {
    char tmp[MSGSIZE];
    strcpy(tmp, message);

    char *ptr = strtok(tmp, " ");
    *pid = atoi(ptr);

    ptr = strtok(NULL, " ");
    *priority = atoi(ptr);

    ptr = strtok(NULL, " ");
    memcpy(filename, ptr, strlen(ptr) + 1);
}