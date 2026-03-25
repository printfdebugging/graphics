#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct string *string_create(const char *data) {
    struct string *string = malloc(sizeof(struct string));
    if (!string) {
        fprintf(stderr, "failed to allocate memory for struct String");
        return NULL;
    }

    *string = (struct string) { 0 };
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

struct string *string_create_from_file(const char *path) {
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

    struct string *string = string_create(NULL);
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
        string_destroy(string);
        return NULL;
    }

    string->data[string->length] = '\0';
    if (fclose(file)) {
        fprintf(stderr, "fclose failed\n");
        string_destroy(string);
        return NULL;
    }

    return string;
}

int string_append(struct string *string, const char *part) {
    if (!part) {
        fprintf(stderr, "cannot append NULL to string\n");
        return 1;
    }

    int length = strlen(part);
    int empty_space = string->capacity - (string->length + 1);
    bool not_enough_space = empty_space < length + 1;

    if (not_enough_space) {
        /* expand the buffer in multiples of `DEFAULT_STRING_CAPACITY` */
        int new_capacity = ceil((string->length + 1 + length + 1) / DEFAULT_STRING_CAPACITY) * DEFAULT_STRING_CAPACITY;
        char *buffer = malloc(new_capacity);
        if (!buffer) {
            fprintf(stderr, "failed to allocate larger buffer for string to append");
            return 1;
        }

        memcpy(buffer, string->data, string->length);
        buffer[string->length] = '\0';

        /* free the old memory and point to the new larger buffer */
        free(string->data);
        string->data = buffer;
        string->capacity = new_capacity;
    }

    string->data[string->length] = '\n';
    memcpy(string->data + string->length + 1, part, length + 1);
    string->length = string->length + 1 + length;
    // todo: see if we need to separate the \0 termination logic

    return 0;
}

int string_append_file(struct string *string, const char *path) {
    struct string *file_contents = string_create_from_file(path);
    if (!file_contents)
        return 1;

    int status = string_append(string, file_contents->data);
    free(file_contents);
    return status;
}

void string_destroy(struct string *string) {
    if (string->data)
        free(string->data);
    free(string);
}
