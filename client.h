#pragma once

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "server.h"
#include "message_queue.h"

struct client_info
{
    int client_thread_status;
    int client_qid;
    int client_pid;
    int client_priority;
    FILE *client_ptr_file;
    int client_file_size;
    char client_file_name[MSGSIZE];
};

int client(int s_qid, int c_priority, const char *c_file_name);

void client_send_info(struct client_info *c_info, struct message_object *mBuffer, int c_pid, const char *c_file_name);

void *client_exit_handler(void *c_info);
