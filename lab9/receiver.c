#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_sem"

struct shared_data {
    time_t current_time;
    pid_t sender_pid;
};

int shm_fd = -1;
sem_t *sem = NULL;
struct shared_data *data = NULL;

void cleanup(void) {  
    if (data != NULL) {
        munmap(data, sizeof(struct shared_data));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    if (sem != NULL) {
        sem_close(sem);
    }
}

void handle_signal(int sig) {
    cleanup();
    exit(0);
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error: Shared memory does not exist or cannot be opened");
        return EXIT_FAILURE;
    }

    data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        cleanup();
        return EXIT_FAILURE;
    }

    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Error: Semaphore does not exist or cannot be opened");
        cleanup();
        return EXIT_FAILURE;
    }

    char time_str[100];

    while (1) {
        sem_wait(sem);

        pid_t receiver_pid = getpid();
        time_t current_time;
        time(&current_time);
        strftime(time_str, sizeof time_str, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

        printf("[pid: %d] Receiver current time: %s\n", receiver_pid, time_str);

        char received_time_str[100];
        strftime(received_time_str, sizeof received_time_str, "%Y-%m-%d %H:%M:%S", localtime(&data->current_time));

        printf("[pid %d] Received data: sender pid = %d, time = %s\n", receiver_pid, data->sender_pid, received_time_str);

        sem_post(sem);

        sleep(3);
    }

    cleanup();

    return 0;
}