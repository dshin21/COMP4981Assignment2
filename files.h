#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "message_queue.h"
#include "threads.h"

FILE *open_file(const char *filename, const char *flags);

int close_file(FILE **fp);

//int close_file_unsafe(FILE **fp);

size_t read_file(FILE *file, struct msgbuf *msg);
