#pragma once

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "message_queue.h"
#include "threads.h"

struct params {
    int *pRunning;
    int qid;
    int pid;
    int priority;
};

int client(int server_qid, int client_priority, const char *client_file_name);

struct msgbuf client_send_info(const char *client_file_name, struct params *p, int pid, struct msgbuf *mBuffer);


void *client_control(void *params);