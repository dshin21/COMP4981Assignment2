#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "message_queue.h"

#define ERROR_FILE "ERROR_FILE"

FILE *open_file(const char *filename, const char *flag);

int close_file(FILE *fp);

size_t read_file(FILE *fp, struct message_object *message_obj);
