#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "engine/core/defines.h"

#define DEFAULT_SHADER_OPTIONS \
	.has_normals = true

enum shader_category {
	shader_category_model,
	shader_category_font,
	shader_category_count,
};

struct shader_options {
	b8 has_normals;
};

struct shader {
	u32 program;

	struct shader_options options;
	enum shader_category category;

	/** This helps avoid using the shader too early. It is marked as `false` in
	 * `shader_init_with_options` and as `true` in `shader_load_from_file` once
	 * the shader has been compiled and linked properly.
	 *
	 * Ideally the shader's are allocated in a batch and the memory is cleared
	 * to `0` after allocation, so this would be set to `false` by default, but
	 * that's just an implicit assumption, a garbage value might as well set it
	 * to `true`.
	 */
	b8 initialized;
};

i8 shader_init_with_options(struct shader *shader, struct shader_options options, enum shader_category category);
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
