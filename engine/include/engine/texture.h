#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"

#include "engine/core/defines.h"

struct texture {
	u32 texture;
	u8 texture_index;

	/* todo: create a enum for texture type, surely there can't be many and an enum would be much better than strings. */
	const char *type;
	/* todo: use this in the code and also document these properly*/
	const char *path;
};

struct texture *texture_create();
struct texture *texture_create_from_file(const char *path);
i8 texture_load(struct texture *texture, void *data, i32 width, i32 height, GLenum format, GLenum data_type, i32 internal_format, b8 generate_mipmaps);
i8 texture_load_from_file(struct texture *texture, const char *path);
void texture_destroy(struct texture *texture);

/* note that we don't have wrappers for binding textures to various texture
 * units and for activating the textures. */

#endif
