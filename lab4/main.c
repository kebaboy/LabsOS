#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

mode_t parse_symbolic_mode(const char *mode_str, mode_t old_mode) {
    mode_t new_mode = old_mode;
    mode_t mask = 0;
    int who = 0;
    int op = 0;

    while (*mode_str) {
        if (*mode_str == 'u' || *mode_str == 'g' || *mode_str == 'o' || *mode_str == 'a') {
            if (*mode_str == 'u') {
                who = S_IRUSR | S_IWUSR | S_IXUSR;
            } else if (*mode_str == 'g') {
                who = S_IRGRP | S_IWGRP | S_IXGRP;
            } else if (*mode_str == 'o') {
                who = S_IROTH | S_IWOTH | S_IXOTH;
            } else if (*mode_str == 'a') {
                who = S_IRWXU | S_IRWXG | S_IRWXO;
            }
            mode_str++;
        }

        if (*mode_str == '+' || *mode_str == '-' || *mode_str == '=') {
            op = (*mode_str == '+') ? 1 : (*mode_str == '-') ? -1 : 0;
            mode_str++;
        }

        while (*mode_str == 'r' || *mode_str == 'w' || *mode_str == 'x') {
            if (*mode_str == 'r') {
                mask |= S_IRUSR | S_IRGRP | S_IROTH;
            } else if (*mode_str == 'w') {
                mask |= S_IWUSR | S_IWGRP | S_IWOTH;
            } else if (*mode_str == 'x') {
                mask |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
            mode_str++;
        }

        if (op == 1) {
            new_mode |= (mask & who);
        } else if (op == -1) {
            new_mode &= ~(mask & who);
        } else if (op == 0) {
            new_mode = (new_mode & ~who) | (mask & who);
        }

        mask = 0;
        who = 0;
    }

    return new_mode;
}

mode_t parse_numeric_mode(const char *mode_str) {
    if (mode_str == NULL || *mode_str == '\0') {
        fprintf(stderr, "Error: Invalid mode string (empty string)\n");
        exit(EXIT_FAILURE);
    }

    for (const char *ptr = mode_str; *ptr != '\0'; ptr++) {
        if (*ptr < '0' || *ptr > '7') {
            fprintf(stderr, "Error: Invalid character '%c' in numeric mode string\n", *ptr);
            exit(EXIT_FAILURE);
        }
    }

    mode_t new_mode = strtol(mode_str, NULL, 8);

    if (new_mode == 0 && mode_str[0] != '0') {
        fprintf(stderr, "Error: Invalid numeric mode (must be an octal number)\n");
        exit(EXIT_FAILURE);
    }

    return new_mode;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: mychmod <mode> <file>\n");
        exit(EXIT_FAILURE);
    }

    const char *mode_str = argv[1];
    const char *file = argv[2];
    struct stat file_stat;

    if (stat(file, &file_stat) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    mode_t new_mode;
    if (mode_str[0] >= '0' && mode_str[0] <= '9') {
        new_mode = parse_numeric_mode(mode_str);
    } else {
        new_mode = parse_symbolic_mode(mode_str, file_stat.st_mode);
    }

    if (chmod(file, new_mode) == -1) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }

    return 0;
}
