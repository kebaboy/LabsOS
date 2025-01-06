#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define SHM_SIZE 64
#define SHM_LOCK_FILE "/tmp/shm_lock_file"

char *shared_memory_address = NULL;
int shared_memory_id = -1;

void cleanup() {
    if (shared_memory_address != NULL) {
        shmdt(shared_memory_address);
    }
}

void handle_signal(int signal) {
    cleanup();
    exit(0);
}

int main() {
    key_t shared_memory_key = ftok(SHM_LOCK_FILE, 1);
    if (shared_memory_key == -1) {
        perror("ftok error (shared memory key not found)");
        return EXIT_FAILURE;
    }

    shared_memory_id = shmget(shared_memory_key, SHM_SIZE, 0666);
    if (shared_memory_id == -1) {
        perror("shmget error (shared memory not found)");
        return EXIT_FAILURE;
    }

    shared_memory_address = (char *)shmat(shared_memory_id, NULL, 0);
    if (shared_memory_address == (void *)-1) {
        perror("shmat error");
        return EXIT_FAILURE;
    }

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    while (1) {
        char local_time_buffer[SHM_SIZE] = {0};
        time_t current_time;
        time(&current_time);
        strftime(local_time_buffer, sizeof(local_time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

        printf("[pid: %d] Receiver current time: %s\n", getpid(), local_time_buffer);
        printf("[pid: %d] Received data: %s\n", getpid(), shared_memory_address);

        sleep(3);
    }

    cleanup();
    return 0;
}