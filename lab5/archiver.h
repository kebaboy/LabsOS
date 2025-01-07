#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <sys/stat.h>

#define BUFFER_SIZE 1024

struct FileHeader {
    char filename[128];
    struct stat file_stat;
};

void print_help();
void archive_file(const char *archive_name, const char *filename);
void extract_file(const char *archive_name, const char *filename);
void list_archive(const char *archive_name);

#endif