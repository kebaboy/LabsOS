#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void list_dir(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (!dir) {
        fprintf(stderr, "Error: Cannot open directory '%s'\n", path);
        return;
    }

    char **files = NULL;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
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
    const char *path = (argc > 1) ? argv[1] : ".";
    list_dir(path);
    return 0;
}