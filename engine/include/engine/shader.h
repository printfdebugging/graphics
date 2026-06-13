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

	/** An identifier for the type of shader. Helps allocate separate chunks/arenas
	 * for separate kinds of shaders, makes it fast to iterate over and compare the
	 * shader options to check if we already have a shader with these optinos.
	 */
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

	/** Shader uniforms. Set to `-1` in `shader_init_with_options` and set
	 * through `shader_set_uniform` which checks for `-1` and caches the valid
	 * `glGetUniformLocation` return values.
	 *
	 * These names are same as the variable names in the shaders, just prefixed
	 * with `uniform_` to make their purpose explicit. `shader_set_uniform`
	 * generates code assuming this prefixed relationship.
	 */
	i32 uniform_model;
	i32 uniform_view;
	i32 uniform_projection;
};

i8 shader_init_with_options(struct shader *shader, struct shader_options options, enum shader_category category);

/** This might seem unnecessary at first, but this helps us avoid
 * the costly shader program switches by comparing the current shader
 * program `shader->program`. If they are same, then it skips the
 * `glUseProgram` call.
 */
void shader_use(struct shader *shader);

void shader_destroy(struct shader *shader);
i8 shader_load_from_sources(struct shader *shader, const char **vertex_sources, i32 vertex_sources_count, const char **fragment_sources, i32 fragment_sources_count);

#define shader_set_uniform(shader, name, type, ...)                                                         \
	{                                                                                                   \
		if (shader->uniform_##name != -1) {                                                         \
			glUniform##type(shader->uniform_##name, __VA_ARGS__);                               \
		} else {                                                                                    \
			shader->uniform_##name = glGetUniformLocation(shader->program, #name);              \
			if (shader->uniform_##name != -1) {                                                 \
				glUniform##type(shader->uniform_##name, __VA_ARGS__);                       \
			} else {                                                                            \
				fprintf(stderr, "no uniform named '%s' found in shader->program\n", #name); \
			}                                                                                   \
		}                                                                                           \
	}

#endif
