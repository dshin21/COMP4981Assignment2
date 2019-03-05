/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.c - A Client program using Message Queues.
--
-- PROGRAM:     MessageQueueApp
--
-- FUNCTIONS:
--              int client(const int s_qid, const int c_priority, const char *c_file_name)
--              void client_send_info(struct client_info *c_info, struct message_object *mBuffer, int c_pid, const char *c_file_name)
--              void *client_exit_handler(void *c_info)
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
--              This is the main entry point for the client program and contains all functionalities related to
--              client operations.
----------------------------------------------------------------------------------------------------------------------*/
#include "client.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    client
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int client(const int s_qid, const int c_priority, const char *c_file_name)
--
-- RETURNS:     int; 0 for successful termination, 1 for otherwise.
--
-- NOTES:
--              Entry point to the client program.
--              This function creates a thread that monitors user input of "s", and terminates the program if it is
--              entered by the user.
--              Furthermore, it initializes client_info to store all the information needed by the client:
--                  client's thread status, process id, priority, and the server's message queue id.
--              The function also initially sends all the information about this client.
----------------------------------------------------------------------------------------------------------------------*/
int client(const int s_qid, const int c_priority, const char *c_file_name)
{
    pthread_t thread_exit_watcher;
    struct client_info c_info;
    struct message_object msg_obj;

    c_info.client_thread_status = 1;
    c_info.client_qid = s_qid;
    c_info.client_pid = (int)getpid();
    c_info.client_priority = c_priority;

    if (pthread_create(&thread_exit_watcher, NULL, client_exit_handler, (void *)&c_info))
    {
        perror("Error: creating thread");
        return 1;
    }

    client_send_info(&c_info, &msg_obj, c_info.client_pid, c_file_name);

    while (c_info.client_thread_status)
    {
        memset(&msg_obj, 0, sizeof(struct message_object));
        if (read_message(s_qid, c_info.client_pid, &msg_obj, BLOCKING) <= 0)
        {
            sched_yield();
            continue;
        }

        if (!strcmp(msg_obj.mtext, ERROR_FILE))
        {
            printf("Error: Reading file\n");
            fflush(stdout);
        }
        else
        {
            printf("%s", msg_obj.mtext);
            fflush(stdout);
        }
    }

    fflush(stdout);

    pthread_join(thread_exit_watcher, 0);
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    client_send_info
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void client_send_info(struct client_info *c_info, struct message_object *mBuffer, int c_pid, const char *c_file_name)
--                  c_info: a pointer to a client_info structure; used to store client's information
--                  mBuffer: a pointer to a message_object structure; used to send message to the server
--                  c_pid: integer indicating the client's process id
--                  c_file_name: a string indicating the client's file name
--
-- RETURNS:     void
--
-- NOTES:
--              This function sends the client information needed to establish a connect to the server.
--              The function stores all the information into a message_object structure and sends it to the server.
----------------------------------------------------------------------------------------------------------------------*/
void client_send_info(struct client_info *c_info, struct message_object *mBuffer, int c_pid, const char *c_file_name)
{
    memset(mBuffer, 0, sizeof(struct message_object));
    sprintf(mBuffer->mtext, "%d %d %s", c_pid, c_info->client_priority, c_file_name);
    mBuffer->mlen = (int)strlen(mBuffer->mtext);
    mBuffer->mtype = CLIENT_TO_SERVER;

    if (send_message(c_info->client_qid, mBuffer) == -1)
        perror("Error: Writing to message queue");
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    client_exit_handler
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void *client_exit_handler(void *c_info)
--                  c_info: a pointer to a client_info structure; used to store client's information
--
-- RETURNS:     NULL
--
-- NOTES:
--              This function continuously monitors the command-line to see if the user has inputted "s" for stop.
--                  If it sees an "s", terminates the program.
----------------------------------------------------------------------------------------------------------------------*/
void *client_exit_handler(void *c_info)
{
    char temp[128];
    char user_input[128];

    struct client_info *p = c_info;
    int *is_running = &p->client_thread_status;

    while (*is_running)
    {
        if (fgets(temp, 128, stdin) && sscanf(temp, "%s", user_input) == 1 && !strcmp(user_input, "s"))
        {
            kill(0, SIGINT);
            *is_running = 0;
        }
        sched_yield();
    }
    return NULL;
}
