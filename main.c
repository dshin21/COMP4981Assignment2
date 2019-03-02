#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 2) cli_guide();
    printf("in main\n");

    if (strcmp(argv[1], "server") == 0) {
        printf("in server\n");
        return server_entry();
    } else if (strcmp(argv[1], "client") == 0) {
        printf("in client\n");
//        if (argc != 4) return cli_guide();
        client_entry(argv);
    }
    return 0;
}

void client_entry(char *const argv[]) {
    printf("in client_entry\n");

    int server_qid = atoi(argv[2]);
    int client_priority;
    const char *client_file_name = argv[4];

    if (!strcmp(argv[3], "S")) {
        client_priority = PRIORITY_SMALL;
    } else if (!strcmp(argv[3], "M")) {
        client_priority = PRIORITY_MEDIUM;
    } else if (!strcmp(argv[3], "L")) {
        client_priority = PRIORITY_LARGE;
    } else {
        cli_guide();
        exit(0);
    }

    if (client(server_qid, client_priority, (char *) client_file_name) != 0) {
        perror("Error with client");
        exit(1);
    }
}

int cli_guide() {
    printf("\nHow to use the MessageQueueApp: ./MessageQueueApp [server | client <server qid> [S|M|L] <filename>]\n");
    return 0;
}