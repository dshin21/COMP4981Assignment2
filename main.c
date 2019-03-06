/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: main.c - A Client-Server program using Message Queues.
--
-- PROGRAM:     MessageQueueApp
--
-- FUNCTIONS:
--              int main(int argc, char *argv[])
--              void client_entry(char *const argv[])
--              int cli_guide()
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
--              This is the main entry point for the program.
--              The user can either execute the program as server or client.
--                  Server: ./MessageQueueApp server
--                  Client: ./MessageQueueApp client <server qid> [S|M|L] <filename>
----------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    main
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int main(int argc, char *argv[])
--                  argc: number of command-line arguments
--                  argv: array of command line arguments
--
-- RETURNS:     int; 0 for successful termination, 1 for otherwise.
--
-- NOTES:
--              Entry point to the program.
--              Parses the user-specified command-line arguments to determine if the program should execute as a 
--              server or client.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    if (argc < 2)
        cli_guide();

    if (strcmp(argv[1], "server") == 0)
    {
        return server_entry();
    }
    else if (strcmp(argv[1], "client") == 0)
    {
        if (argc != 5)
            return cli_guide();
        client_entry(argv);
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    client_entry
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void client_entry(char *const argv[])
--                  argv: array of command line arguments
--
-- RETURNS:     void
--
-- NOTES:
--              Entry point to the client program.
--              Parses the user-specified command-line arguments for clients and executes the client.
----------------------------------------------------------------------------------------------------------------------*/
void client_entry(char *const argv[])
{
    int server_qid = atoi(argv[2]);
    int client_priority;
    const char *client_file_name = argv[4];

    if (!strcmp(argv[3], "S"))
    {
        client_priority = PRIORITY_SMALL;
    }
    else if (!strcmp(argv[3], "M"))
    {
        client_priority = PRIORITY_MEDIUM;
    }
    else if (!strcmp(argv[3], "L"))
    {
        client_priority = PRIORITY_LARGE;
    }
    else
    {
        cli_guide();
        exit(0);
    }

    if (client(server_qid, client_priority, (char *)client_file_name) != 0)
    {
        exit(1);
    }
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    cli_guide
--
-- DATE:        Feb. 25, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   int cli_guide()
--
-- RETURNS:     0. For indicating a successful return.
--
-- NOTES:
--              This function displays the instructions to use the program, if executed using invalid number of 
--              command-line arguments.
----------------------------------------------------------------------------------------------------------------------*/
int cli_guide()
{
    printf("\nHow to use the MessageQueueApp: ./MessageQueueApp [server | client <server qid> [S|M|L] <filename>]\n");
    return 0;
}