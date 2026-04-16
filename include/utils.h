#ifndef UTILS_H
#define UTILS_H

#define DEFAULT_STRING_CAPACITY 512.0f

struct String {
    char *data;

    /*
     * `length` is the length of the string, not length
     * of the memory used by the string which would be
     * `length + 1` with that `+1` for the `\0` byte.
     */
    int length;

    /*
     * `capacity` is the total number of bytes the data
     * buffer has been allocated. we allocate it in multiples
     * of `DEFAULT_STRING_CAPACITY`.
     */
    int capacity;
};

struct String *stringCreate(const char *data);
struct String *stringCreateFromFile(const char *path);

/*
 * we always append a `part` to the `string` string.
 * returns 0 on success. `part` is a null terminated string.
 * this function doesn't free part (infact it assumes part
 * to be stack allocated).
 */
int stringAppend(struct String *string, const char *part);
int stringAppendFile(struct String *string, const char *path);
void stringDestroy(struct String *string);

#endif
