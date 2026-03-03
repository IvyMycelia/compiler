#include <stdio.h>
#include <stdlib.h>

#include "file.h"

char* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        perror("Could not open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = malloc(length + 1);
    buffer[length] = '\0';
    if (!buffer) {
        perror("Could not read file");
        return NULL;
    }

    if (!fclose(f)) {
        perror("Could not close file");
        return NULL;
    }

    return buffer;
}