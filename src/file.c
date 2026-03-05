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

    if (length < 0) {
        fclose(f);
        return NULL;
    }

    char* buffer = malloc(length + 1);
    if (!buffer) {
        perror(RED "Memory allocation failed");
        fclose(f);
        return NULL;
    }

    size_t read = fread(buffer, 1, length, f);
    if (read != length) {
        perror(RED "Could not read file");
        free(buffer);
        fclose(f);
        return NULL;
    }
    
    buffer[length] = '\0';
    if (fclose(f)) {
        perror(RED "Could not close file");
        return NULL;
    }
    
    return buffer;
}