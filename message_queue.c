#include "message_queue.h"


/*
 * <sys/msg.h>: int msgget(key_t key, int msgflg);
 * @params: key - semaphore id
 *          msgflg - IPC_CREAT: tells msget to create a msg Q for val. key if not exists
 *                   0660: r/w for user & group
 * @desc: perform atomic operates on user-defined array of semaphore operation structures
 * @return: T: msg Q identifier > 0
 *          F: -1 && errno set
 */
int queue_open(const key_t keyval) {
    int qid; // msg Q id
    qid = msgget(keyval, IPC_CREAT | 0660);
    return qid;
}

/*
 * <sys/msg.h>: int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
 * @params: msqid - msg Q id
 *          *msgp - ptr to caller-defined struct (msgbuf)
 *          msgsz - msg size
 *          msgflg - 0: calling thread suspends exec. untill:
 *              1. condition responsible for suspension is gone -> msg sent
 *              2. msqid removed from the system -> return -1
 * @desc: sends a msg to the Q associated with the msqid
 * @return: T: # bytes written to the msg Q
 *          F: -1
 */
int send_message(const int msg_qid, struct message_object *qbuf) {
    int result;
    result = msgsnd(msg_qid, qbuf, qbuf->mlen, 0);  //writes to msg Q w/ blocking call
    return result;
}

/*
 * <sys/msg.h>: ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
 * @params: msqid - msg Q id
 *          *msgp - ptr to caller-defined struct (msgbuf)
 *          msgsz - msg size
 *          msgtyp - 0: 1st msg on Q received
 *                  >0: first msg of type msgtyp received
 *                  <0: lowest type <= abs(msgtyp) received
 *          msgflg - IPC_NOWAIT: process sleep if not exist suitable msg on the Q, comes back when it does -> always returns
 * @desc: read a msg from the Q associated with msquid and place it in msgp
 * @return: T: # bytes written to the mtext(msgbuf)
 *          F: -1
 */
int read_message(const int qid, const long type, struct message_object *qbuf, int read_type) {
    int r_type = 0;
    if (read_type == BLOCKING) r_type = 0;
    if (read_type == NONBLOCKING) r_type = IPC_NOWAIT;
    int result;
    result = (int) msgrcv(qid, qbuf, MSGSIZE, type, r_type);
    qbuf->mlen = result;
    return result;
}

/*
 * <sys/msg.h>: int msgctl(int msqid, int cmd, struct msqid_ds *buf); [Option End]
 * @params: msqid - msg Q id
 *          cmd - IPC_RMID: removes msqid and it's structures
 * @desc: removes a msg Q
 * @return: T: 0
 *          F: -1
 */
int remove_queue(const int qid) {
    int result;
    result = msgctl(qid, IPC_RMID, 0);
    return result;
}
