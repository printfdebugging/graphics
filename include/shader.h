#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

struct Shader {
        unsigned int program;
};

struct Shader *shaderCreate();

void shaderDestroy(struct Shader *shader);
int  shaderLoadFromFile(struct Shader *shader, const char *vpath, const char *fpath);

#define shaderSetUniform(shader, name, type, ...)                                                  \
        {                                                                                          \
                int var_##location = glGetUniformLocation(shader->program, name);                  \
                if (var_##location == -1) {                                                        \
                        fprintf(stderr, "no uniform named '%s' found in shader->program\n", name); \
                } else {                                                                           \
                        if (var_##location != -1) glUniform##type(var_##location, __VA_ARGS__);    \
                }                                                                                  \
        }

#endif
