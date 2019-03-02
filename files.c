#include "files.h"

pthread_mutex_t mutex;

FILE *open_file(const char *filename, const char *flags) {
    FILE *fp;
    pthread_mutex_lock(&mutex);
    fp = fopen(filename, flags);
    pthread_mutex_unlock(&mutex);
    return fp;
}

int close_file(FILE **fp) {
    int result = -1;
    pthread_mutex_lock(&mutex);
    if (*fp != NULL) {
        result = fclose(*fp);
        if (!result) {
            *fp = NULL;
        }
    }
    pthread_mutex_unlock(&mutex);
    return result;
}

size_t read_file(FILE *file, struct message_object *msg) {
    size_t result = 0;

    pthread_mutex_lock(&mutex);
    if (file != NULL) {
        memset(msg->mtext, 0, sizeof(char) * MSGSIZE);
        result = fread(msg->mtext, sizeof(char), MSGSIZE - 1, file);
        msg->mtext[result] = '\0';
        msg->mlen = (int) (result + 1);
    }
    pthread_mutex_unlock(&mutex);

    return result;
}
