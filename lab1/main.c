#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void list_dir(const char *path, int show_all) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (!dir) {
        fprintf(stderr, "Error: Cannot open directory '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    char **files = NULL;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.')
            continue;
        files = realloc(files, sizeof(char *) * (count + 1));
        files[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dir);

    qsort(files, count, sizeof(char *), compare);

    for (int i = 0; i < count; i++) {
        printf("%s\n", files[i]);
        free(files[i]);
    }
    free(files);
}

int main(int argc, char *argv[]) {
    int opt;
    int show_all = 0;

    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";
    list_dir(path, show_all);
    return 0;
}