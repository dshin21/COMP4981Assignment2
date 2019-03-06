/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: message_queue.c - A Client program using Message Queues.
--
-- PROGRAM:     MessageQueueApp
--
-- FUNCTIONS:
--              int queue_open(const key_t keyval)
--              int send_message(const int msg_qid, struct message_object *qbuf)
--              int read_message(const int qid, const long type, struct message_object *qbuf, int read_type)
--              int remove_queue(const int qid)
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
--              This file contains all functionalities related to message queues.
----------------------------------------------------------------------------------------------------------------------*/
#include "message_queue.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    queue_open
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int queue_open(const key_t keyval)
--                  keyval: a key_t value to use to open up a message queue
--
-- RETURNS:     returns a positive integer on success, -1 otherwise
--
-- NOTES:
--              This function opens up a new message queue
----------------------------------------------------------------------------------------------------------------------*/
int queue_open(const key_t keyval)
{
  return msgget(keyval, IPC_CREAT | 0660);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    send_message
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int send_message(const int msg_qid, struct message_object *qbuf)
--                  msg_qid: the integer id of the message queue to use
--                  qbuf: a pointer to a message_object structure; used to send message to the client.
--
-- RETURNS:     returns the number of bytes sent to the message queue, -1 on failure.
--
-- NOTES:
--              This function sends a new message to the message queue.
----------------------------------------------------------------------------------------------------------------------*/
int send_message(const int msg_qid, struct message_object *qbuf)
{
  return msgsnd(msg_qid, qbuf, qbuf->mlen, 0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    read_message
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int read_message(const int msg_qid, const long type, struct message_object *qbuf, int read_type)
--                  msg_qid: the integer id of the message queue to use
--                  type: a long indicating the type of the message
--                  qbuf: a pointer to a message_object structure; used to send message to the client.
--                  read_type: type of read:
--                    either BLOCKING or NONBLOCKING
--
-- RETURNS:     returns the number of bytes read from the message queue, -1 on failure.
--
-- NOTES:
--              This function reads messages from the message queue in either blocking or non-blocking mode.
----------------------------------------------------------------------------------------------------------------------*/
int read_message(const int msg_qid, const long type, struct message_object *qbuf, int read_type)
{
  int r_type = 0;
  if (read_type == BLOCKING)
    r_type = 0;
  if (read_type == NONBLOCKING)
    r_type = IPC_NOWAIT;
  int result;
  result = (int)msgrcv(msg_qid, qbuf, MSGSIZE, type, r_type);
  qbuf->mlen = result;
  return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    remove_queue
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int remove_queue(const int msg_qid)
--                  msg_qid: the integer id of the message queue to use
--
-- RETURNS:     returns a positive integer on success, -1 on failure.
--
-- NOTES:
--              This function removes the message queue.
----------------------------------------------------------------------------------------------------------------------*/
int remove_queue(const int msg_qid)
{
  return msgctl(msg_qid, IPC_RMID, 0);
}
