#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "message_queue.h"
#include "files.h"
#include "server.h"
#include "client.h"

#define PRIORITY_SMALL 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_LARGE 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void client_entry(char *const *argv);

int cli_guide();

//TODO: remove all comments