#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10

char shared_array[ARRAY_SIZE];
pthread_rwlock_t rwlock;
int counter = 0;

void *reader(void *arg) {
    int tid = *((int *)arg);

    while (1) {
        if (pthread_rwlock_rdlock(&rwlock) != 0) {
            perror("Error locking rwlock for reading in reader thread");
            pthread_exit(NULL);
        }

        printf("Reader %d: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%c ", shared_array[i]);
        }
        printf("\n");

        if (pthread_rwlock_unlock(&rwlock) != 0) {
            perror("Error unlocking rwlock in reader thread");
            pthread_exit(NULL);
        }

        usleep(100000);
    }

    return NULL;
}

void *writer(void *arg) {
    while (1) {
        if (pthread_rwlock_wrlock(&rwlock) != 0) {
            perror("Error locking rwlock for writing in writer thread");
            pthread_exit(NULL);
        }

        for (int i = 0; i < ARRAY_SIZE; i++) {
            shared_array[i] = 'A' + (counter % 26);
        }
        counter++;

        if (pthread_rwlock_unlock(&rwlock) != 0) {
            perror("Error unlocking rwlock in writer thread");
            pthread_exit(NULL);
        }

        usleep(200000);
    }

    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writer_thread;
    int reader_ids[NUM_READERS];

    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        perror("Error initializing rwlock");
        exit(1);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        if (pthread_create(&readers[i], NULL, reader, (void *)&reader_ids[i]) != 0) {
            perror("Error creating reader thread");
            exit(1);
        }
    }

    if (pthread_create(&writer_thread, NULL, writer, NULL) != 0) {
        perror("Error creating writer thread");
        exit(1);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        if (pthread_join(readers[i], NULL) != 0) {
            perror("Error joining reader thread");
            exit(1);
        }
    }

    if (pthread_join(writer_thread, NULL) != 0) {
        perror("Error joining writer thread");
        exit(1);
    }

    if (pthread_rwlock_destroy(&rwlock) != 0) {
        perror("Error destroying rwlock");
        exit(1);
    }

    return 0;
}
