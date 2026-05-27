#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/string.h"
#include "core/defines.h"

static u64 string_get_extra_capacity(struct string *string) {
        if (!string || string->capacity == 0)
                return 0;
        if (string->capacity == string->length + 1) /* capacity < length can never happen */
                return 0;
        return string->capacity - (string->length + 1);
}

static u64 string_calculate_new_capacity_for_length(u64 length) {
        return (u64) (ceil((f64) length / DEFAULT_STRING_CAPACITY) * DEFAULT_STRING_CAPACITY);
}

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
        string->capacity = string_calculate_new_capacity_for_length(string->length + 1);
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
        i64 length = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (length < 0) {
                fprintf(stderr, "failed to get the shader file's length: %s\n", path);
                fclose(file);
                return NULL;
        }

        struct string *string = string_create(NULL);
        string->length = (u64) length;
        string->capacity = string_calculate_new_capacity_for_length(string->length + 1);
        string->data = malloc(string->capacity);

        if (!string->data) {
                fprintf(stderr, "failed to allocate memory for string->data to store file %s\n", path);
                fclose(file);
                return NULL;
        }

        u64 read_count = fread(string->data, 1, string->length, file);
        if (read_count < string->length || read_count == 0) {
                fprintf(stderr, "read returned %li which is either 0 or less than %li", read_count, length);
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

i8 string_append(struct string *string, const char *part) {
        if (!part) {
                fprintf(stderr, "cannot append NULL to string\n");
                return 1;
        }

        u64 length = strlen(part);
        u64 extra_capacity = string_get_extra_capacity(string);
        b8 not_enough_space = extra_capacity < length + 1;

        if (not_enough_space) {
                /* expand in multiples of `DEFAULT_STRING_CAPACITY` */
                u64 new_capacity = string_calculate_new_capacity_for_length(string->length + 1 + length + 1);
                char *buffer = malloc(new_capacity);
                if (!buffer) {
                        fprintf(stderr, "failed to allocate larger buffer for string to append");
                        return 1;
                }

                if (string->data) {
                        memcpy(buffer, string->data, string->length);
                        buffer[string->length] = '\0';
                        free(string->data);
                }

                string->data = buffer;
                string->capacity = new_capacity;
        }

        string->data[string->length] = '\n';
        memcpy(string->data + string->length + 1, part, length + 1);
        string->length = string->length + 1 + length;
        // todo: see if we need to separate the \0 termination logic

        return 0;
}

i8 string_append_file(struct string *string, const char *path) {
        struct string *file_contents = string_create_from_file(path);
        if (!file_contents)
                return 1;

        i8 status = string_append(string, file_contents->data);
        free(file_contents);
        return status;
}

void string_destroy(struct string *string) {
        if (string->data)
                free(string->data);
        free(string);
}
