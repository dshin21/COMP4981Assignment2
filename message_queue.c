#include "message_queue.h"

int queue_open(const key_t keyval) {
    return msgget(keyval, IPC_CREAT | 0660);
}

int send_message(const int msg_qid, struct message_object *qbuf) {
    return msgsnd(msg_qid, qbuf, qbuf->mlen, 0);
}

int read_message(const int qid, const long type, struct message_object *qbuf, int read_type) {
    int r_type = 0;
    if (read_type == BLOCKING) r_type = 0;
    if (read_type == NONBLOCKING) r_type = IPC_NOWAIT;
    int result;
    result = (int) msgrcv(qid, qbuf, MSGSIZE, type, r_type);
    qbuf->mlen = result;
    return result;
}

int remove_queue(const int qid) {
    return msgctl(qid, IPC_RMID, 0);
}
