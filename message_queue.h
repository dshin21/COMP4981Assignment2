#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define CLIENT_TO_SERVER 1
#define BLOCKING 0
#define NONBLOCKING 1
#define MSGHEADERSIZE (sizeof(int) + sizeof(long))
#define MSGSIZE (4096 - MSGHEADERSIZE)

struct message_object {
    long mtype;
    char mtext[MSGSIZE];
    int mlen;
};

int queue_open(key_t keyval);

int send_message(int msg_qid, struct message_object *qbuf);

int read_message(int qid, const long type, struct message_object *qbuf, int read_type);

int remove_queue(int qid);