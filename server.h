#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include "message_queue.h"
#include "semaphore.h"
#include "files.h"
#include "client.h"

static int server_qid;
static int semaphore_id;

int server_entry();

void *server_exit_handler(void *exit_watcher);

void server_exit_cleanup(int code);

int server_accept_client(struct client_info *c_info);

int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info);

int server_send_err_msg(struct message_object *s_buffer, struct client_info *c_info);

void server_get_client_info(const char *message, int *pid, int *priority, char *filename);
