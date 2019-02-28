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

int clnt(const int qid, const int priority);

void *client_control(void *params);