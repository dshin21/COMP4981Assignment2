#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[]) {
    int qid;

    if (argc < 2) {
        printUsage();
    }

    // Server
    if (!strcmp(argv[1], "server")) {
        return srvr();
    }
        // Client
    else {
        int priority = 2;
        char *p;

        if (argc != 3) {
            printUsage();
            return 0;
        }

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
            printUsage();
            exit(0);
        }

        if (clnt(qid, priority) != 0) {
            perror("Error with client");
            exit(1);
        }
    }

    return 0;
}


void printUsage() {
    printf("Usage: ./assign2 [server|[high | normal | low] qid]\n");
}