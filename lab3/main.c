#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigint_handler(int sig) {
    printf("[pid: %d] Received SIGINT, signal number: %d\n", getpid(), sig);
    exit(EXIT_SUCCESS);
}

void sigterm_handler(int sig, siginfo_t *info, void *context) {
    printf("[pid: %d] Received SIGTERM, signal number: %d\n", getpid(), sig);
    exit(EXIT_SUCCESS);
}

void on_exit_handler(void) {
    printf("[pid: %d] Process is exiting...\n", getpid());
}

int main() {
    atexit(on_exit_handler);

    signal(SIGINT, sigint_handler);

    struct sigaction sa;
    sa.sa_sigaction = sigterm_handler;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        printf("[pid: %d] Child process\n", getpid());
        sleep(5);
    } else {
        int status;
        waitpid(child_pid, &status, 0);
        printf("[pid: %d] Parent process, child pid: %d\n", getpid(), child_pid);
    }

    return 0;
}
