#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#define SHM_SIZE 64
#define SHM_LOCK_FILE "/tmp/shm_lock_file"

char *shared_memory_address = NULL;
int shared_memory_id = -1;

void cleanup() {
    if (shared_memory_address != NULL) {
        shmdt(shared_memory_address);
    }

    if (shared_memory_id != -1) {
        shmctl(shared_memory_id, IPC_RMID, NULL);
    }

    unlink(SHM_LOCK_FILE);
}

void handle_signal(int signal) {
    cleanup();
    exit(0);
}

int main() {
    int lock_file_descriptor = open(SHM_LOCK_FILE, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (lock_file_descriptor == -1) {
        perror("Sender already running or cannot create lock file");
        return EXIT_FAILURE;
    }
    close(lock_file_descriptor);

    key_t shared_memory_key = ftok(SHM_LOCK_FILE, 1);
    if (shared_memory_key == -1) {
        perror("ftok error");
        unlink(SHM_LOCK_FILE);
        return EXIT_FAILURE;
    }

    shared_memory_id = shmget(shared_memory_key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shared_memory_id == -1) {
        perror("shmget error");
        unlink(SHM_LOCK_FILE);
        return EXIT_FAILURE;
    }

    shared_memory_address = (char *)shmat(shared_memory_id, NULL, 0);
    if (shared_memory_address == (void *)-1) {
        perror("shmat error");
        shmctl(shared_memory_id, IPC_RMID, NULL);
        unlink(SHM_LOCK_FILE);
        return EXIT_FAILURE;
    }

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    while (1) {
        char time_buffer[SHM_SIZE] = {0};
        char formatted_message[2 * SHM_SIZE] = {0};

        time_t current_time;
        time(&current_time);

        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        snprintf(formatted_message, sizeof(formatted_message), "time = %s, PID = %d", time_buffer, getpid());

        strncpy(shared_memory_address, formatted_message, SHM_SIZE - 1);

        sleep(3);
    }

    cleanup();
    return 0;
}