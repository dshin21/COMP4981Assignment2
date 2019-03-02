#pragma once

#include <sys/types.h>  // includes definitions for types
#include <sys/ipc.h>    // inter-process communication access structure
#include <sys/msg.h>    // message queue structures
#include "threads.h"

#define CLIENT_TO_SERVER 1
#define BLOCKING 0
#define NONBLOCKING 1
#define CLIENT_TO_SERVER 1
#define MSGHEADERSIZE (sizeof(int) + sizeof(long))
#define MSGSIZE (4096 - MSGHEADERSIZE)

struct message_object {
    long mtype; // msg type; > 0
    char mtext[MSGSIZE]; // msg function
    int mlen;   // number of octets
};

int queue_open(const key_t keyval);

int send_message(const int msg_qid, struct message_object *qbuf);

int read_message(const int qid, const long type, struct message_object *qbuf, int read_type);

int remove_queue(const int qid);