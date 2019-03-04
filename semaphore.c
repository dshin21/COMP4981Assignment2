#include "semaphore.h"

int create_semaphore(key_t key) {
    int sid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    return sid;
}

int remove_semaphore(int sid) {
    return semctl(sid, 0, IPC_RMID, 0);
}

void P(int sid) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;

    if (semop(sid, &buf, 1) == -1) perror("Error: P() ");
}

void V(int sid) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(sid, &buf, 1) == -1) perror("Error: V() ");
}