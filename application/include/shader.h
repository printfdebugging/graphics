#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

struct shader {
    unsigned int program;
};

struct shader *shader_create();
void shader_destroy(struct shader *shader);
int shader_load_from_file(struct shader *shader, const char *vpath, const char *fpath);

#define shader_set_uniform(shader, name, type, ...)                                    \
    {                                                                                  \
        int var_##location = glGetUniformLocation(shader->program, name);              \
        if (var_##location == -1) {                                                    \
            fprintf(stderr, "no uniform named '%s' found in shader->program\n", name); \
        } else {                                                                       \
            if (var_##location != -1) glUniform##type(var_##location, __VA_ARGS__);    \
        }                                                                              \
    }

#endif
