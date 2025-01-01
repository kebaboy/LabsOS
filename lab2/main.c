#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO make options struct
void process_file(FILE *file, int show_numbers, int show_nonblank_numbers, int show_ends) {
    char buffer[4096];
    int line_number = 1;
    int non_blank_line = 1;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (show_numbers || show_nonblank_numbers) {
            if (show_nonblank_numbers && strlen(buffer) > 1) {
                printf("%6d\t", non_blank_line++);
            } else if (show_numbers) {
                printf("%6d\t", line_number++);
            }
        }
        
        fputs(buffer, stdout);
        if (show_ends && buffer[strlen(buffer) - 1] == '\n') {
            printf("$");
        }
    }
}

int main(int argc, char *argv[]) {
    int opt;
    // TODO make options struct
    int show_numbers = 0;
    int show_nonblank_numbers = 0;
    int show_ends = 0;

    while ((opt = getopt(argc, argv, "nbE")) != -1) {
        switch (opt) {
            case 'n':
                show_numbers = 1;
                break;
            case 'b':
                show_nonblank_numbers = 1;
                break;
            case 'E':
                show_ends = 1;
                break;
            default:
                fprintf(stderr, "Usage: mycat [-n] [-b] [-E] [file...]");
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        process_file(stdin, show_numbers, show_nonblank_numbers, show_ends);
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                fprintf(stderr, "Error: Cannot open empty file\n");
                continue;
            }
            process_file(file, show_numbers, show_nonblank_numbers, show_ends);
            fclose(file);
        }
    }

    return 0;
}
