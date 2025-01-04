#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10

char shared_array[ARRAY_SIZE];
pthread_cond_t cond;
pthread_mutex_t mutex;
int counter = 0;

void *reader(void *arg) {
    int tid = *((int *)arg);

    while (1) {
        if (pthread_mutex_lock(&mutex) != 0) {
            perror("Error locking mutex in reader thread");
            pthread_exit(NULL);
        }

        if (pthread_cond_wait(&cond, &mutex) != 0) {
            perror("Error waiting on condition variable in reader thread");
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        printf("Reader %d: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%c ", shared_array[i]);
        }
        printf("\n");

        if (pthread_mutex_unlock(&mutex) != 0) {
            perror("Error unlocking mutex in reader thread");
            pthread_exit(NULL);
        }

        usleep(100000);
    }

    return NULL;
}

void *writer(void *arg) {
    while (1) {
        if (pthread_mutex_lock(&mutex) != 0) {
            perror("Error locking mutex in writer thread");
            pthread_exit(NULL);
        }

        for (int i = 0; i < ARRAY_SIZE; i++) {
            shared_array[i] = 'A' + (counter % 26);
        }
        counter++;

        if (pthread_cond_broadcast(&cond) != 0) {
            perror("Error broadcasting condition variable in writer thread");
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        if (pthread_mutex_unlock(&mutex) != 0) {
            perror("Error unlocking mutex in writer thread");
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

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Error initializing mutex");
        exit(1);
    }

    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("Error initializing condition variable");
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

    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Error destroying mutex");
        exit(1);
    }

    if (pthread_cond_destroy(&cond) != 0) {
        perror("Error destroying condition variable");
        exit(1);
    }

    return 0;
}