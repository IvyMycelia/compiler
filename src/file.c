#include <stdio.h>
#include <stdlib.h>

#include "ANSI.h"

#include "file.h"

char* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        perror(RED "Could not open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = malloc(length + 1);
    buffer[length] = '\0';
    if (!buffer) {
        perror(RED "Could not read file");
        return NULL;
    }

    if (!fclose(f)) {
        perror(RED "Could not close file");
        return NULL;
    }

    return buffer;
}