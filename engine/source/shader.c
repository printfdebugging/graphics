#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

#include "engine/shader.h"
#include "engine/primitive.h"
#include "engine/core/defines.h"
#include "engine/core/string.h"

static const char *shader_variable_names[] = {
	[PRIMITIVE_ATTRIBUTE_POSITION] = "in_position",
	[PRIMITIVE_ATTRIBUTE_COLOR] = "in_color",
	[PRIMITIVE_ATTRIBUTE_UV] = "in_uv",
	[PRIMITIVE_ATTRIBUTE_NORMAL] = "in_normal",
};

#if defined(EMSCRIPTEN)
static const char *version = "#version 300 es\n";
#else
static const char *version = "#version 330 core\n";
#endif

static const char *shader_float_precision_declaration =
    "#ifdef GL_ES\n"
    "    precision mediump float;\n"
    "#endif\n";

static i8 shader_compiled_successfully(u32 shader, const char *filepath) {
	i32 success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success)
		return 0;

	i32 info_log_len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);

	char info_log[info_log_len];
	glGetShaderInfoLog(shader, info_log_len, NULL, info_log);
	fprintf(stderr, "failed to compile shader: %s: %s\n", filepath, info_log);
	return 1;
}

static i8 shader_linked_successfully(u32 program) {
	i32 success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success)
		return 0;

	i32 info_log_len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);

	char info_log[info_log_len];
	glGetProgramInfoLog(program, info_log_len, NULL, info_log);
	fprintf(stderr, "failed to link shader program: %s\n", info_log);
	return 1;
}

static void shader_bind_variable_names(u32 program) {
	for (enum primitive_attribute i = PRIMITIVE_ATTRIBUTE_POSITION; i < PRIMITIVE_ATTRIBUTE_COUNT; ++i) {
		glBindAttribLocation(program, i, shader_variable_names[i]);
	}
}

i8 shader_load_from_file(struct shader *shader, const char *vpath, const char *fpath) {
	/* read and compile vertex shader */

	// TODO: instead stringify the part to append first and
	// then when loading the shader file allocate enough space
	// for both the stringified options and the shader file's
	// contents. then printf both the strings to the buffer.
	// https://gist.github.com/nitrix/386d3acc9a6ef6ea63dac79393ad6163
	struct string *vsource = string_create(NULL);
	if (!vsource)
		return 1;
	if (string_append(vsource, version)) {
		string_destroy(vsource);
		return 1;
	}
	if (string_append(vsource, shader_float_precision_declaration)) {
		string_destroy(vsource);
		return 1;
	}
	if (string_append_file(vsource, vpath)) {
		string_destroy(vsource);
		return 1;
	}

	u32 vshader = glCreateShader(GL_VERTEX_SHADER);
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
	if (string_append(fsource, shader_float_precision_declaration))
		return 1;
	if (string_append_file(fsource, fpath))
		return 1;

	u32 fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, (const char **) &fsource->data, NULL);
	glCompileShader(fshader);
	string_destroy(fsource);

	if (shader_compiled_successfully(fshader, fpath))
		return 1;

	/* create shader program */
	u32 sprogram = glCreateProgram();
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

i8 shader_init_with_options(struct shader *shader, struct shader_options options) {
	shader->options = options;
	return 0;
}

void shader_destroy(struct shader *shader) {
	glDeleteProgram(shader->program);
	free(shader);
}
