#include "archiver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    if (argc < 4) {
        print_help();
        exit(EXIT_FAILURE);
    }

    const char *archive_name = argv[1];
    const char *option = argv[2];

    if (strcmp(option, "-i") == 0 || strcmp(option, "--input") == 0) {
        for (int i = 3; i < argc; i++) {
            archive_file(archive_name, argv[i]);
        }
    } else if (strcmp(option, "-e") == 0 || strcmp(option, "--extract") == 0) {
        for (int i = 3; i < argc; i++) {
            extract_file(archive_name, argv[i]);
        }
    } else if (strcmp(option, "-s") == 0 || strcmp(option, "--stat") == 0) {
        list_archive(archive_name);
    } else {
        print_help();
        exit(EXIT_FAILURE);
    }

    return 0;
}
