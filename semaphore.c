/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: semaphore.c - A Client program using Message Queues.
--
-- PROGRAM:     MessageQueueApp
--
-- FUNCTIONS:
--              int create_semaphore(key_t key)
--              int remove_semaphore(int sid)
--              void P(int sid)
--              void V(int sid)
--
-- DATE:        Feb. 25, 2019
--
-- REVISIONS:   None
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- NOTES:
--              This file contains all functionalities related to semaphores.
----------------------------------------------------------------------------------------------------------------------*/
#include "semaphore.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    create_semaphore
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int create_semaphore(key_t key)
--                  keyval: a key_t value to use to create a semaphore.
--
-- RETURNS:     returns the semaphore id on success, -1 otherwise.
--
-- NOTES:
--              This function creates a new semaphore.
----------------------------------------------------------------------------------------------------------------------*/
int create_semaphore(key_t key)
{
    int sid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    return sid;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    remove_semaphore
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int remove_semaphore(int sid)
--                  keyval: a key_t value to use to remove a semaphore.
--
-- RETURNS:     returns the a positive integer on success, -1 otherwise.
--
-- NOTES:
--              This function removes a semaphore.
----------------------------------------------------------------------------------------------------------------------*/
int remove_semaphore(int sid)
{
    return semctl(sid, 0, IPC_RMID, 0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    P
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void P(int sid)
--                  sid: id of the semaphore to operate on.
--
-- RETURNS:     void.
--
-- NOTES:
--              This function decreaments (aquires) the semaphore
----------------------------------------------------------------------------------------------------------------------*/
void P(int sid)
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;

    if (semop(sid, &buf, 1) == -1)
        perror("Error: P() ");
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    V
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void V(int sid)
--                  sid: id of the semaphore to operate on.
--
-- RETURNS:     void.
--
-- NOTES:
--              This function increments (releases) the semaphore
----------------------------------------------------------------------------------------------------------------------*/
void V(int sid)
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(sid, &buf, 1) == -1)
        perror("Error: V() ");
}