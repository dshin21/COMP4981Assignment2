#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 2) cli_guide();
    printf("in main");

    if (strcmp(argv[1], "server") == 0) {
        printf("in server");
        return srvr();
    } else if (strcmp(argv[1], "client") == 0) {
        printf("in client");
        if (argc != 3) return cli_guide();
        entry_client(argv);
    }
    return 0;
}

void entry_client(char *const *argv) {
    int qid;
    int priority;
    char *p;

    // grab qid of server
    qid = atoi(argv[2]);

    // grab priority
    p = argv[1];
    if (!strcmp(p, "high")) {
        priority = HIGH;
    } else if (!strcmp(p, "normal")) {
        priority = NORMAL;
    } else if (!strcmp(p, "low")) {
        priority = LOW;
    } else {
        cli_guide();
        exit(0);
    }

    if (clnt(qid, priority) != 0) {
        perror("Error with client");
        exit(1);
    }
}


int cli_guide() {
    printf("\nHow to use the MessageQueueApp: ./MessageQueueApp [server | client [S|M|L] <filename>]\n");
    return 0;
}