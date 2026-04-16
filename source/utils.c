#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct String *stringCreate(const char *data) {
    struct String *string = malloc(sizeof(struct String));
    if (!string) {
        fprintf(stderr, "failed to allocate memory for struct String");
        return NULL;
    }

    *string = (struct String) { 0 };
    if (!data)
        return string;

    string->length = strlen(data);
    string->capacity = ceil((string->length + 1) / DEFAULT_STRING_CAPACITY) * DEFAULT_STRING_CAPACITY;
    string->data = malloc(string->capacity);

    if (!string->data) {
        fprintf(stderr, "failed to allocate memory for string->data\n");
        return NULL;
    }

    string->data = strcpy(string->data, data);
    return string;
}

struct String *stringCreateFromFile(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "failed to read shader file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length < 0) {
        fprintf(stderr, "failed to get the shader file's length: %s\n", path);
        fclose(file);
        return NULL;
    }

    struct String *string = stringCreate(NULL);
    string->length = length;
    string->capacity = ceil((string->length + 1) / DEFAULT_STRING_CAPACITY) * DEFAULT_STRING_CAPACITY;
    string->data = malloc(string->capacity);

    if (!string->data) {
        fprintf(stderr, "failed to allocate memory for string->data to store file %s\n", path);
        fclose(file);
        return NULL;
    }

    int readCount = fread(string->data, 1, string->length, file);
    if (readCount < string->length || readCount == 0) {
        fprintf(stderr, "read returned %i which is either 0 or less than %li", readCount, length);
        fclose(file);
        stringDestroy(string);
        return NULL;
    }

    string->data[string->length] = '\0';
    if (fclose(file)) {
        fprintf(stderr, "fclose failed\n");
        stringDestroy(string);
        return NULL;
    }

    return string;
}

int stringAppend(struct String *string, const char *part) {
    if (!part) {
        fprintf(stderr, "cannot append NULL to string\n");
        return 1;
    }

    int length = strlen(part);
    int emptySpace = string->capacity - (string->length + 1);
    bool notEnoughSpace = emptySpace < length + 1;

    if (notEnoughSpace) {
        /* expand the buffer in multiples of `DEFAULT_STRING_CAPACITY` */
        int newCapacity = ceil((string->length + 1 + length + 1) / DEFAULT_STRING_CAPACITY) * DEFAULT_STRING_CAPACITY;
        char *buffer = malloc(newCapacity);
        if (!buffer) {
            fprintf(stderr, "failed to allocate larger buffer for string to append");
            return 1;
        }

        memcpy(buffer, string->data, string->length);
        buffer[string->length] = '\0';

        /* free the old memory and point to the new larger buffer */
        free(string->data);
        string->data = buffer;
        string->capacity = newCapacity;
    }

    string->data[string->length] = '\n';
    memcpy(string->data + string->length + 1, part, length + 1);
    string->length = string->length + 1 + length;
    // todo: see if we need to separate the \0 termination logic

    return 0;
}

int stringAppendFile(struct String *string, const char *path) {
    struct String *fileContents = stringCreateFromFile(path);
    if (!fileContents)
        return 1;

    int status = stringAppend(string, fileContents->data);
    free(fileContents);
    return status;
}

void stringDestroy(struct String *string) {
    if (string->data)
        free(string->data);
    free(string);
}
