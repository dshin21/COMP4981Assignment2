#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "threads.h"
#include "message_queue.h"
#include "semaphore.h"
#include "files.h"
#include "client.h"

static int server_qid;
static int semaphore_id;

int server_entry();

void *server_control(void *params);

void abort_cleanup(int code);

int acceptClients(struct client_info *c_info);

void parseClientRequest(const char *message, int *pid, int *priority, char *filename);
