#pragma once

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "message_queue.h"
#include "threads.h"

struct client_info {
    int *client_thread_status;
    int client_qid;
    int client_pid;
    int client_priority;
    FILE *client_ptr_file;
    int client_file_size;
    char client_file_name[];
};

int client(int server_qid, int client_priority, const char *client_file_name);

void client_send_info(struct client_info *p, struct message_object *mBuffer, int pid, const char *client_file_name);

void *client_control(void *params);
