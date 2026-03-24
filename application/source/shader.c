#include "logger.h"
#include "shader.h"
#include "mesh.h"
#include "utils.h"

#include "glad/glad.h"

#include <stdio.h>
#include <stdlib.h>

/* clang-format off */
static const char *shader_variable_names[] = {
    [MESH_ATTRIBUTE_POSITION] = "in_position",
    [MESH_ATTRIBUTE_COLOR]    = "in_color",
    [MESH_ATTRIBUTE_UV]       = "in_uv",
    [MESH_ATTRIBUTE_NORMAL]   = "in_normal",
};
/* clang-format on */

#if defined(EMSCRIPTEN)
static const char *version = "#version 300 es\n";
#else
static const char *version = "#version 330 core\n";
#endif

static const char *shader_float_position_declaration = "#ifdef GL_ES\n"
                                                       "    precision mediump float;\n"
                                                       "#endif\n";

static int shader_compiled_successfully(unsigned int shader, const char *filepath) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return 0;

    int infoLogLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    char infoLog[infoLogLen];
    glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
    fprintf(stderr, "failed to compile shader: %s: %s\n", filepath, infoLog);
    return 1;
}

static int shader_linked_successfully(unsigned int program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return 0;

    int infoLogLen;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);

    char infoLog[infoLogLen];
    glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
    fprintf(stderr, "failed to link shader program: %s\n", infoLog);
    return 1;
}

static void shader_bind_variable_names(unsigned int program) {
    for (enum mesh_attribute i = MESH_ATTRIBUTE_POSITION; i < MESH_ATTRIBUTE_COUNT; ++i)
        glBindAttribLocation(program, i, shader_variable_names[i]);
}

struct shader *shader_create() {
    struct shader *shader = malloc(sizeof(struct shader));
    if (!shader) {
        fprintf(stderr, "failed to allocate memory for shader\n");
        return NULL;
    }

    *shader = (struct shader) { 0 };
    return shader;
}

int shader_load_from_file(struct shader *shader, const char *vpath, const char *fpath) {
    /* read and compile vertex shader */

    // TODO: instead stringify the part to append first and
    // then when loading the shader file allocate enough space
    // for both the stringified options and the shader file's
    // contents. then printf both the strings to the buffer.
    // https://gist.github.com/nitrix/386d3acc9a6ef6ea63dac79393ad6163
    struct string *vsource = string_create(NULL);
    if (!vsource)
        return 1;
    if (string_append(vsource, version))
        return 1;
    if (string_append(vsource, shader_float_position_declaration))
        return 1;
    if (string_append_file(vsource, vpath))
        return 1;

    unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, (const char **) &vsource->data, NULL);
    glCompileShader(vshader);
    string_destroy(vsource);

    if (shader_compiled_successfully(vshader, vpath))
        return 1;

    /* read and compile fragment shader */
    // TODO: instead stringify the part to append first and
    // then when loading the shader file allocate enough space
    // for both the stringified options and the shader file's
    // contents. then printf both the strings to the buffer.
    // https://gist.github.com/nitrix/386d3acc9a6ef6ea63dac79393ad6163
    struct string *fsource = string_create(NULL);
    if (!fsource)
        return 1;
    if (string_append(fsource, version))
        return 1;
    if (string_append(fsource, shader_float_position_declaration))
        return 1;
    if (string_append_file(fsource, fpath))
        return 1;

    unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, (const char **) &fsource->data, NULL);
    glCompileShader(fshader);
    string_destroy(fsource);

    if (shader_compiled_successfully(fshader, fpath))
        return 1;

    /* create shader program */
    unsigned int sprogram = glCreateProgram();
    if (sprogram == 0) {
        fprintf(stderr, "failed to create shader program\n");
        return 1;
    }

    glAttachShader(sprogram, vshader);
    glAttachShader(sprogram, fshader);

    /* bind attribute locations and link */
    shader_bind_variable_names(sprogram);
    glLinkProgram(sprogram);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    if (shader_linked_successfully(sprogram))
        return 1;

    glUseProgram(sprogram);

    shader->program = sprogram;
    return 0;
}

void shader_destroy(struct shader *shader) {
    glDeleteProgram(shader->program);
    free(shader);
}
