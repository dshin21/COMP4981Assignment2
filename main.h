#define HIGH    16
#define NORMAL  4
#define LOW     1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message_queue.h"
#include "files.h"

#include "server.h"
#include "client.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void printUsage();