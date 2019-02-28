#pragma once

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "files.h"
#include "message_queue.h"
#include "threads.h"
#include "semaphore.h"

int srvr();

void *server_control(void *params);

void catchSig(int sig);

int acceptClients(int *pPid, int *pPriority, char *pFile);

void parseClientRequest(const char *message, int *pid, int *priority, char *filename);
