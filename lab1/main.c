#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void print_permissions(mode_t mode) {
    char perms[11] = "----------";
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    printf("%s ", perms);
}

void list_dir(const char *path, int show_all, int long_list) {
    struct dirent *entry;
    struct stat st;
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
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i]);
        if (stat(fullpath, &st) == -1) {
            perror("stat");
            continue;
        }

        if (long_list) {
            print_permissions(st.st_mode);
            printf("%2hu %s %s %5lld ",
                   st.st_nlink,
                   getpwuid(st.st_uid)->pw_name,
                   getgrgid(st.st_gid)->gr_name,
                   st.st_size);
            char timebuf[80];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
            printf("%s ", timebuf);
        }

        printf("%s\n", files[i]);
        free(files[i]);
    }
    free(files);
}

int main(int argc, char *argv[]) {
    int opt;
    int show_all = 0, long_list = 0;

    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                long_list = 1;
                break;
            case 'a':
                show_all = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-a] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";
    list_dir(path, show_all, long_list);
    return 0;
}
