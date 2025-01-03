#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 256

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }   

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

        if (read(pipefd[0], buffer, BUFFER_SIZE) == -1) {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
        }

        printf("[pid: %d] Child process current time: %s\n", getpid(), time_str);
        printf("[pid: %d] Received message: %s\n", getpid(), buffer);

        close(pipefd[0]);
    } else {
        close(pipefd[0]);

        sleep(5);

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
        snprintf(buffer, BUFFER_SIZE, "[pid: %d] Parent process current time: %s", getpid(), time_str);

        if (write(pipefd[1], buffer, strlen(buffer)) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);

        wait(NULL);
    }

    return 0;
}