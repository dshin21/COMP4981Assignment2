#pragma once

#include <sys/types.h>  // includes definitions for types
#include <sys/ipc.h>    // inter-process communication access structure
#include <sys/msg.h>    // message queue structures
#include "threads.h"

#define S_TO_C 1
#define C_TO_S 1
#define MSGHEADERSIZE (sizeof(int) + sizeof(long))
#define MSGSIZE (4096 - MSGHEADERSIZE)

struct msgbuf {
    long mtype; // msg type; > 0
    char mtext[MSGSIZE]; // msg function
    int mlen;   // number of octets
} msg;

int open_queue(const key_t keyval);

int send_message(const int msg_qid, struct msgbuf *qbuf);

int read_message(const int qid, const long type, struct msgbuf *qbuf);

int read_message_blocking(const int qid, const long type, struct msgbuf *qbuf);

int remove_queue(const int qid);