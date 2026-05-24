#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"

struct texture {
        unsigned int texture;
        short int texture_index;
        const char *type;
};

struct texture *texture_create();
struct texture *texture_create_from_file(const char *path);
int texture_load(struct texture *texture, void *data, unsigned int width, unsigned int height, GLenum format, GLenum data_type, GLenum internal_format, GLboolean generate_mipmaps);
int texture_load_from_file(struct texture *texture, const char *path);
void texture_destroy(struct texture *texture);

/* note that we don't have wrappers for binding textures to various texture
 * units and for activating the textures. */

#endif
