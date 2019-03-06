#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "message_queue.h"
#include "server.h"
#include "client.h"

#define PRIORITY_SMALL 5
#define PRIORITY_MEDIUM 25
#define PRIORITY_LARGE 50

void client_entry(char *const *argv);

int cli_guide();
