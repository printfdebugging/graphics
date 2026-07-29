#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

#include "engine/shader.h"
#include "engine/model/primitive.h"
#include "engine/core/defines.h"

/** Cache for the current shader program. used in `shader_use` to
 * avoid unnecessary `glUseProgram` calls. */
static u32 current_program = 0;

static status __shader_get_compile_status(u32 shader_object);
static status __shader_get_link_status(u32 program);

/*
 * todo: remove this, we do it manually now, atleast for the harfbuzz shaders. maybe later
 * we standardize a few things there as well, though i think that won't be a good idea
 * static const char *shader_variable_names[] = {
 * 	[PRIMITIVE_ATTRIBUTE_POSITION] = "in_position",
 * 	[PRIMITIVE_ATTRIBUTE_COLOR] = "in_color",
 * 	[PRIMITIVE_ATTRIBUTE_UV] = "in_uv",
 * 	[PRIMITIVE_ATTRIBUTE_NORMAL] = "in_normal",
 * };
 *
 * static void shader_bind_variable_names(u32 program);
 *
 * static void shader_bind_variable_names(u32 program) {
 * 	for (enum primitive_attribute i = PRIMITIVE_ATTRIBUTE_POSITION; i < PRIMITIVE_ATTRIBUTE_COUNT; ++i) {
 * 		glBindAttribLocation(program, i, shader_variable_names[i]);
 * 	}
 * }
 */

static status __shader_get_compile_status(u32 shader_object) {
	i32 compile_status;
	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compile_status);

	if (compile_status) {
		return status_success;
	}

	i32 length;
	glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length);

	char log[length];
	glGetShaderInfoLog(shader_object, length, NULL, log);
	fprintf(stderr, "failed to compile shader, error message: %s\n", log);
	return status_failure;
}

static status __shader_get_link_status(u32 program) {
	i32 link_status;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);

	if (link_status) {
		return status_success;
	}

	i32 length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

	char log[length];
	glGetProgramInfoLog(program, length, NULL, log);
	fprintf(stderr, "failed to link shader program: %s\n", log);
	return status_failure;
}

status shader_load_from_sources(struct shader *shader, const char **vertex_sources, i32 vertex_sources_count, const char **fragment_sources, i32 fragment_sources_count) {
	u32 vertex_shader_object;
	u32 fragment_shader_object;
	u32 program;
	status compile_error;
	status link_error;

	vertex_shader_object = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_object, vertex_sources_count, vertex_sources, NULL);
	glCompileShader(vertex_shader_object);

	if (!(compile_error = __shader_get_compile_status(vertex_shader_object))) {
		return status_failure;
	}

	fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_object, fragment_sources_count, fragment_sources, NULL);
	glCompileShader(fragment_shader_object);

	if (!(compile_error = __shader_get_compile_status(fragment_shader_object))) {
		glDeleteShader(vertex_shader_object);
		return status_failure;
	}

	program = glCreateProgram();
	glAttachShader(program, vertex_shader_object);
	glAttachShader(program, fragment_shader_object);
	// note: this shouldn't be done here, it is something which varies per shader.
	// shader_bind_variable_names(program);
	glLinkProgram(program);

	glDeleteShader(vertex_shader_object);
	glDeleteShader(fragment_shader_object);

	if (!(link_error = __shader_get_link_status(program))) {
		glDeleteProgram(program);
		return status_failure;
	}

	shader->program = program;
	shader->initialized = true;
	return status_success;
}

status shader_init_with_options(struct shader *shader, struct shader_options options, enum shader_category category) {
	shader->options = options;
	shader->category = category;
	shader->initialized = false;

	shader->uniform_model = -1;
	shader->uniform_view = -1;
	shader->uniform_projection = -1;
	return status_success;
}

void shader_use(struct shader *shader) {
	if (shader->program != current_program) {
		glUseProgram(shader->program);
		current_program = shader->program;
	}
}

void shader_destroy(struct shader *shader) {
	glDeleteProgram(shader->program);
	free(shader);
}
