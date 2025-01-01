#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_file(FILE *file, const char *pattern) {
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, pattern)) {
            fputs(buffer, stdout);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: mygrep pattern [file...]\n");
        exit(EXIT_FAILURE);
    }

    const char *pattern = argv[1];

    if (argc == 2) {
        process_file(stdin, pattern);
    } else {
        for (int i = 2; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                fprintf(stderr, "Error: Cannot open empty file\n");
                continue;
            }
            process_file(file, pattern);
            fclose(file);
        }
    }

    return 0;
}
