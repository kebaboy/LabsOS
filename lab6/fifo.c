#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fifo_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char *fifo_path = argv[1];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[BUFFER_SIZE];

    if (mkfifo(fifo_path, 0666) == -1) {
        perror("fifo err");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        int fd = open(fifo_path, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open FIFO in child");
            unlink(fifo_path);
            exit(EXIT_FAILURE);
        }

        if (read(fd, buffer, BUFFER_SIZE) == -1) {
            perror("Read from fifo failed");
            unlink(fifo_path);
            exit(EXIT_FAILURE);
        }

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

        printf("[pid: %d] Child process current time: %s\n", getpid(), time_str);
        printf("[pid: %d] Received message: %s\n", getpid(), buffer);

        close(fd);
        unlink(fifo_path);
    } else {
        sleep(5);

        int fd = open(fifo_path, O_WRONLY);
        if (fd == -1) {
            perror("Failed to open FIFO in parent");
            wait(NULL);
            unlink(fifo_path);
            exit(EXIT_FAILURE);
        }

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

        snprintf(buffer, BUFFER_SIZE, "[pid: %d] Parent process current time: %s", getpid(), time_str);

        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("Write to fifo failed");
        }

        close(fd);
        wait(NULL);
    }

    return 0;
}
