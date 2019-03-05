/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: server.c - A Server program using Message Queues.
--
-- PROGRAM:     MessageQueueApp
--
-- FUNCTIONS:
--              int server_entry()
--              int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info)
--              int server_send_err_msg(struct message_object *s_buffer, struct client_info *c_info)
--              void *server_exit_handler(void *exit_watcher)
--              void server_exit_cleanup(int code)
--              int server_accept_client(struct client_info *c_info)
--              void server_get_client_info(const char *message, int *pid, int *priority, char *filename)
--              FILE *open_file(const char *filename, const char *flag)
--              int close_file(FILE *fp)
--              size_t read_file(FILE *fp, struct message_object *message_obj)
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
--              This is the main entry point for the server program and contains all functionalities related to
--              server operations.
----------------------------------------------------------------------------------------------------------------------*/
#include "server.h"

pthread_mutex_t mutex;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_entry
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int server_entry()
--
-- RETURNS:     int; 0 for successful termination, 1 for otherwise.
--
-- NOTES:
--              Entry point to the server program.
--              This function creates a thread that monitors user input of "s", and terminates the program if it is
--              entered by the user.
--              Furthermore, it initializes all resource needed by the server: message queue and semaphore and waits
--              for a new client connection.
----------------------------------------------------------------------------------------------------------------------*/
int server_entry()
{
    struct message_object s_buffer;
    struct client_info c_info;

    pthread_t thread_exit_watcher;
    int exit_watcher = 1;
    signal(SIGINT, server_exit_cleanup);

    if (pthread_create(&thread_exit_watcher, NULL, server_exit_handler, &exit_watcher))
    {
        perror("Error: creating thread");
        return 1;
    }

    if ((server_qid = queue_open((int)getpid())) == -1)
    {
        perror("Error: opening message queue");
        return 1;
    }
    else
    {
        printf("\nCommand for client: ./MessageQueueApp client %d [S|M|L] <filename>\n", server_qid);
        fflush(stdout);
    }

    if ((semaphore_id = create_semaphore((int)getpid())) < 0)
    {
        perror("Error: creating semaphore");
        return 1;
    }

    V(semaphore_id);

    while (exit_watcher)
    {
        if (!server_accept_client(&c_info))
        {
            sched_yield();
            continue;
        }

        if (!fork())
        {
            c_info.client_ptr_file = open_file(c_info.client_file_name, "r");
            if (c_info.client_ptr_file == NULL)
                return server_send_err_msg(&s_buffer, &c_info);
            return server_send_file_to_client(&s_buffer, &c_info);
        }
    }

    server_exit_cleanup(0);

    pthread_join(thread_exit_watcher, 0);
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_send_file_to_client
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info)
--                  s_buffer: a pointer to a message_object structure; used to send message to the client.
--                  client_info: a pointer to a client_info structure; used to store client's information such as:
--                      client's process id, file pointer, priority, file size.
--
-- RETURNS:     int; 0 for successful termination, 1 for otherwise.
--
-- NOTES:
--              This function reads the file provided by the client by priority.
--              If utilizes a semaphore to ensure synchronization and sends the message to the client 
--              "client_priority" many times until the entire file is sent.
----------------------------------------------------------------------------------------------------------------------*/
int server_send_file_to_client(struct message_object *s_buffer, struct client_info *c_info)
{
    int ret_val = 0;
    s_buffer->mtype = c_info->client_pid;

    printf("child started PID: %d\n", getpid());

    while (!feof(c_info->client_ptr_file))
    {
        P(semaphore_id);
        int i;
        for (i = 0; i < c_info->client_priority; i++)
        {
            c_info->client_file_size = (int)read_file(c_info->client_ptr_file, s_buffer);

            if (c_info->client_file_size == 0)
            {
                ret_val = 0;
            }

            if (c_info->client_file_size < 0)
            {
                perror("Error: Reading from file");
                ret_val = 1;
            }

            if (send_message(server_qid, s_buffer) == -1)
            {
                perror("Error: Sending message");
                ret_val = 1;
            }
        }

        V(semaphore_id);

        sched_yield();

        if (ret_val)
        {
            break;
        }
    }
    close_file(c_info->client_ptr_file);
    printf("%d -  finished and exiting\n", getpid());
    return ret_val;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_send_err_msg
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int server_send_err_msg(struct message_object *s_buffer, struct client_info *c_info)
--                  s_buffer: a pointer to a message_object structure; used to send message to the client.
--                  client_info: a pointer to a client_info structure; used to store client's information such as:
--                      client's process id, file pointer, priority, file size.
--
-- RETURNS:     1. For indicating an unsuccessful return.
--
-- NOTES:
--              This function sends an error message to the client, indicating that there was a problem reading the 
--              file received from the client.
----------------------------------------------------------------------------------------------------------------------*/
int server_send_err_msg(struct message_object *s_buffer, struct client_info *c_info)
{
    s_buffer->mtype = c_info->client_pid;
    strcpy(s_buffer->mtext, ERROR_FILE);
    s_buffer->mlen = (int)strlen(ERROR_FILE);

    if (send_message(server_qid, s_buffer) == -1)
    {
        perror("Error: could not send to client");
    }

    printf("Error: reading file\n");
    return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_exit_handler
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void *server_exit_handler(void *exit_watcher)
--                  exit_watcher: a pointer to an integer value indicating the status of the thread.
--
-- RETURNS:     NULL
--
-- NOTES:
--              This function continuously monitors the command-line to see if the user has inputted "s" for stop.
--                  If it sees an "s", terminates the program.
----------------------------------------------------------------------------------------------------------------------*/
void *server_exit_handler(void *exit_watcher)
{
    int *is_running = (int *)exit_watcher;
    char temp[128];
    char user_input[128];

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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_exit_cleanup
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void server_exit_cleanup(int code)
--                  code: the code indicating the type of the signal.
--
-- RETURNS:     void
--
-- NOTES:
--              This function frees up the acquired resources: message queue and semaphore.
--              If the user terminates the program using "ctrl + c", the program abruptly exits.
----------------------------------------------------------------------------------------------------------------------*/
void server_exit_cleanup(int code)
{
    remove_semaphore(semaphore_id);
    remove_queue(server_qid);
    if (code == SIGINT)
        exit(0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_accept_client
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int server_accept_client(struct client_info *c_info)
--                  c_info: a pointer to a client_info structure; used to store client's information such as:
--                      client's process id, file name, priority, file size.
--
-- RETURNS:     returns 1 if the client info was successfully recieved, 0 other wise.
--
-- NOTES:
--              This function creates a new message object as a buffer to reads the message sent by the client and 
--              initializes the client_info structure for later use.
----------------------------------------------------------------------------------------------------------------------*/
int server_accept_client(struct client_info *c_info)
{
    struct message_object buffer;
    memset(&buffer, 0, sizeof(struct message_object));

    if (read_message(server_qid, CLIENT_TO_SERVER, &buffer, BLOCKING) > 0)
    {
        printf("New Client - %s\n", buffer.mtext);

        server_get_client_info(buffer.mtext, &c_info->client_pid, &c_info->client_priority, c_info->client_file_name);
        return 1;
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    server_get_client_info
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void server_get_client_info(const char *message, int *pid, int *priority, char *filename)
--                  message: a string that contains the message sent by the client
--                  pid: a pointer to an integer in the client_info structure that will store client's pid
--                  priority: a pointer to an integer in the client_info structure that will store client's priority
--                  filename: a pointer to a string in the client_info structure that will store client's file name
--
-- RETURNS:     void.
--
-- NOTES:
--              This function reads the message sent by the client and parses the message, for the client's pid, priority,
--              and the file name, storing into the client_info structure for later use.
----------------------------------------------------------------------------------------------------------------------*/
void server_get_client_info(const char *message, int *pid, int *priority, char *filename)
{
    char tmp[MSGSIZE];
    strcpy(tmp, message);

    char *ptr = strtok(tmp, " ");
    *pid = atoi(ptr);

    ptr = strtok(NULL, " ");
    *priority = atoi(ptr);

    ptr = strtok(NULL, " ");
    memcpy(filename, ptr, strlen(ptr) + 1);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    open_file
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   FILE *open_file(const char *filename, const char *flag)
--                  filename: a pointer to a string in the client_info structure that will store client's file name
--                  flag: a string that indicates the method of opening the file:
--                      r: opens the file for reading
--
-- RETURNS:     returns a file descriptor for the client's specified file.
--
-- NOTES:
--              This function uses a mutex to prevent and race conditions and opens the client specified file.
----------------------------------------------------------------------------------------------------------------------*/
FILE *open_file(const char *filename, const char *flag)
{
    FILE *fp;
    pthread_mutex_lock(&mutex);
    fp = fopen(filename, flag);
    pthread_mutex_unlock(&mutex);
    return fp;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    close_file
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int close_file(FILE *fp)
--                  fp: a file pointer the client file
--
-- RETURNS:     returns 0 if the file is successfully closed, -1 otherwise.
--
-- NOTES:
--              This function uses a mutex to prevent and race conditions and closes the client specified file.
----------------------------------------------------------------------------------------------------------------------*/
int close_file(FILE *fp)
{
    int temp = -1;
    pthread_mutex_lock(&mutex);
    if (fp)
        temp = fclose(fp);
    pthread_mutex_unlock(&mutex);
    return temp;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    read_file
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   size_t read_file(FILE *fp, struct message_object *message_obj)
--                  fp: a file pointer the client file
--                  message_obj: a pointer to a message_object structure; used to send message to the client.
--
-- RETURNS:     returns the size of the client's file
--
-- NOTES:
--              This function uses a mutex to prevent and race conditions and reads the client specified file into the
--              message_object structure, to send back to the client.
----------------------------------------------------------------------------------------------------------------------*/
size_t read_file(FILE *fp, struct message_object *message_obj)
{
    size_t size = 0;

    pthread_mutex_lock(&mutex);
    if (fp)
    {
        memset(message_obj->mtext, 0, sizeof(char) * MSGSIZE);
        size = fread(message_obj->mtext, sizeof(char), MSGSIZE - 1, fp);
        message_obj->mtext[size] = '\0';
        message_obj->mlen = (int)(size + 1);
    }
    pthread_mutex_unlock(&mutex);

    return size;
}
