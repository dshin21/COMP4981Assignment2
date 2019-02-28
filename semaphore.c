#include "semaphore.h"

/*
 * @param <sys/types.h> - key_t : Used for XSI interprocess communication
 */
int create_semaphore(key_t key) {
    /*
     * <sys/sem.h>: int semget(key_t key, int nsems, int semflg);
     * @param:  key - key==IPC_PRIVATE
     *          nsems - # semaphores
     *          semflag - 0666: read/write permission
 *                        IPC_CREAT: Create entry if key does not exist
     *                    IPC_EXCL: Fail if key exists
     * @desc:   gets set of semaphores
     * @return: T: non-neg int, semaphore identifier
     *          F: -1 && errno set
*/
    int sid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    return sid;
}

/*
 * @param:  sid - id of semaphore
 */
int remove_semaphore(int sid) {
    /*
    * <sys/sem.h>: int semctl(int semid, int semnum, int cmd, ...);
    * @param:  semid - semaphore id
    *          nsems - # semaphores
    *          cmd - IPC_RMID: remove semaphore w/ id semid
    *          ... - optional, so 0
    * @desc:   semaphore control operations
    * @return: depends on val. of cmd: IPC_RMID returns 0 on success, -1 on error
*/
    return semctl(sid, 0, IPC_RMID, 0);
}

/*
 * <sys/sem.h>: int semop(int semid, struct sembuf *sops, unsigned nsops);
 * @params: semid - semaphore id
 *          *sops - ptr to a user-defined array of semaphore operation structures
 *          nsops - number of structures in the array
 * @desc: perform atomic operates on user-defined array of semaphore operation structures
 * @return: T: 0
 *          F: -1 && errno set
 */
void P(int sid) {
    struct sembuf p;
    p.sem_num = 0;  // semaphore #
    p.sem_op = -1;  // semaphore operation; < 0, so its val. is subtracted from the semaphore; obtains resources that it controls
    p.sem_flg = SEM_UNDO;   // operation flag; automatically undone when the process terminates

    if (semop(sid, &p, 1) == -1) {
        perror("semop P error");
    }
}

/*
 * <sys/sem.h>: int semop(int semid, struct sembuf *sops, unsigned nsops);
 * @params: semid - semaphore id
 *          *sops - ptr to a user-defined array of semaphore operation structures
 *          nsops - number of structures in the array
 * @desc: perform atomic operates on user-defined array of semaphore operation structures
 * @return: T: 0
 *          F: -1 && errno set
 */
void V(int sid) {
    struct sembuf v;
    v.sem_num = 0;  // semaphore #
    v.sem_op = 1;   // semaphore operation; > 0, so its val. is added to the semaphore; returns resources back to the app's sem. set
    v.sem_flg = SEM_UNDO;   // operation flag; automatically undone when the process terminates

    if (semop(sid, &v, 1) == -1) {
        perror("semop V error");
    }
}