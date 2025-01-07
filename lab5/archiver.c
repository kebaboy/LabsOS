#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <utime.h>

void print_help() {
    printf("Usage:\n");
    printf("  ./archiver arch_name -i(--input) file1 [file2 ...]\n");
    printf("  ./archiver arch_name -e(--extract) file1 [file2 ...]\n");
    printf("  ./archiver arch_name -s(--stat)\n");
    printf("  ./archiver -h(--help)\n");
}

void archive_file(const char *archive_name, const char *filename) {
    int archive_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (archive_fd == -1) {
        perror("Failed to open archive");
        return;
    }

    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("Failed to open file");
        close(archive_fd);
        return;
    }

    struct FileHeader header;
    memset(&header, 0, sizeof(header));
    strncpy(header.filename, filename, sizeof(header.filename) - 1);

    if (fstat(file_fd, &header.file_stat) == -1) {
        perror("Failed to get file stat");
        close(file_fd);
        close(archive_fd);
        return;
    }

    if (write(archive_fd, &header, sizeof(header)) != sizeof(header)) {
        perror("Failed to write file header");
        close(file_fd);
        close(archive_fd);
        return;
    }

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Memory allocation error");
        close(file_fd);
        close(archive_fd);
        return;
    }

    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(archive_fd, buffer, bytes_read) != bytes_read) {
            perror("Failed to write file data");
            free(buffer);
            close(file_fd);
            close(archive_fd);
            return;
        }
    }

    if (bytes_read == -1) {
        perror("Error reading file");
    }

    free(buffer);
    close(file_fd);
    close(archive_fd);
}

void extract_file(const char *archive_name, const char *filename) {
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd == -1) {
        perror("Failed to open archive");
        return;
    }

    struct FileHeader header;
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Memory allocation error");
        close(archive_fd);
        return;
    }

    while (read(archive_fd, &header, sizeof(header)) > 0) {
        if (strcmp(header.filename, filename) == 0) {
            int file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, header.file_stat.st_mode);
            if (file_fd == -1) {
                perror("Failed to create file");
                free(buffer);
                close(archive_fd);
                return;
            }

            ssize_t bytes_to_read = header.file_stat.st_size;
            ssize_t bytes_read;
            while (bytes_to_read > 0 && (bytes_read = read(archive_fd, buffer, BUFFER_SIZE)) > 0) {
                if (write(file_fd, buffer, bytes_read) != bytes_read) {
                    perror("Failed to write to extrcted file");
                    free(buffer);
                    close(file_fd);
                    close(archive_fd);
                    return;
                }
                bytes_to_read -= bytes_read;
            }

            if (bytes_read == -1) {
                perror("Error reading archive");
            }

            close(file_fd);
            utime(filename, NULL);
        } else {
            lseek(archive_fd, header.file_stat.st_size, SEEK_CUR);
        }
    }

    free(buffer);
    close(archive_fd);
}

void list_archive(const char *archive_name) {
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd == -1) {
        perror("Failed to open archive");
        return;
    }

    struct FileHeader header;
    while (read(archive_fd, &header, sizeof(header)) > 0) {
        printf("File: %s, Size: %lld bytes, Mode: %o\n",
               header.filename, header.file_stat.st_size, header.file_stat.st_mode);
        lseek(archive_fd, header.file_stat.st_size, SEEK_CUR);
    }
    close(archive_fd);
}