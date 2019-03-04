#include "files.h"

pthread_mutex_t mutex;

FILE *open_file(const char *filename, const char *flag) {
    FILE *fp;
    pthread_mutex_lock(&mutex);
    fp = fopen(filename, flag);
    pthread_mutex_unlock(&mutex);
    return fp;
}

int close_file(FILE *fp) {
    int temp = -1;
    pthread_mutex_lock(&mutex);
    if (fp) temp = fclose(fp);
    pthread_mutex_unlock(&mutex);
    return temp;
}

size_t read_file(FILE *fp, struct message_object *message_obj) {
    size_t size = 0;

    pthread_mutex_lock(&mutex);
    if (fp) {
        memset(message_obj->mtext, 0, sizeof(char) * MSGSIZE);
        size = fread(message_obj->mtext, sizeof(char), MSGSIZE - 1, fp);
        message_obj->mtext[size] = '\0';
        message_obj->mlen = (int) (size + 1);
    }
    pthread_mutex_unlock(&mutex);

    return size;
}
