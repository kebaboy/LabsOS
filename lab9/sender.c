#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>

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
    if (shm_fd != -1) {
        data = mmap(NULL, sizeof(struct shared_data), PROT_READ, MAP_SHARED, shm_fd, 0);
        if (data != MAP_FAILED) {
            printf("Another instance is already running (PID: %d). Exiting...\n", data->sender_pid);
            munmap(data, sizeof(struct shared_data));
        }
        close(shm_fd);
        return EXIT_FAILURE;
    }

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error: Shared memory cannot be created");
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("Error setting size of shared memory");
        cleanup();
        return EXIT_FAILURE;
    }

    data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        cleanup();
        return EXIT_FAILURE;
    }

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Error: Semaphore cannot be created");
        cleanup();
        return EXIT_FAILURE;
    }

    char time_str[100];
    
    while (1) {
        time(&data->current_time);
        data->sender_pid = getpid();

        strftime(time_str, sizeof time_str, "%Y-%m-%d %H:%M:%S", localtime(&data->current_time));

        sem_wait(sem);
        printf("Sending data: time = %s, PID = %d\n", time_str, data->sender_pid);
        sem_post(sem);

        sleep(3);
    }

    cleanup();

    return 0;
}
