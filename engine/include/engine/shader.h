#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "engine/core/defines.h"

#define DEFAULT_SHADER_OPTIONS \
	.has_normals = true

struct shader_options {
	b8 has_normals;
};

struct shader {
	u32 program;
	struct shader_options options;
};

i8 shader_init_with_options(struct shader *shader, struct shader_options options);
void shader_destroy(struct shader *shader);
i8 shader_load_from_file(struct shader *shader, const char *vpath, const char *fpath);

#define shader_set_uniform(shader, name, type, ...)                                                \
	{                                                                                          \
		int var_##location = glGetUniformLocation(shader->program, name);                  \
		if (var_##location == -1) {                                                        \
			fprintf(stderr, "no uniform named '%s' found in shader->program\n", name); \
		} else {                                                                           \
			if (var_##location != -1)                                                  \
				glUniform##type(var_##location, __VA_ARGS__);                      \
		}                                                                                  \
	}

#endif
