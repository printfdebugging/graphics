#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"

enum texture_index {
    TEXTURE0 = 0,
    TEXTURE1 = 1,
    TEXTURE2 = 2,
    TEXTURE3 = 3,
    TEXTURE4 = 4,
    TEXTURE5 = 5,
    TEXTURE6 = 6,
    TEXTURE7 = 7,
    TEXTURE8 = 8,
    TEXTURE9 = 9,
    TEXTURE10 = 10,
    TEXTURE11 = 11,
    TEXTURE12 = 12,
    TEXTURE13 = 13,
    TEXTURE14 = 14,
    TEXTURE15 = 15,
    TEXTURE16 = 16,
    TEXTURE17 = 17,
    TEXTURE18 = 18,
    TEXTURE19 = 19,
    TEXTURE20 = 20,
    TEXTURE21 = 21,
    TEXTURE22 = 22,
    TEXTURE23 = 23,
    TEXTURE24 = 24,
    TEXTURE25 = 25,
    TEXTURE26 = 26,
    TEXTURE27 = 27,
    TEXTURE28 = 28,
    TEXTURE29 = 29,
    TEXTURE30 = 30,
    TEXTURE31 = 31,
    TEXTURE_COUNT = 32,
};

struct texture {
    unsigned int texture;
};

struct texture *texture_create();
int texture_load(struct texture *texture, void *data, unsigned int width, unsigned int height, GLenum format, GLenum data_type, GLenum internal_format, GLboolean generate_mipmaps);
int texture_load_from_file(struct texture *texture, const char *path);
void texture_destroy(struct texture *texture);

/* TODO: later!
int txLoadFromData(
    struct Texture   *texture,
    const char       *path,  // <-- data, width, height, format
    const char       *shVarName,
    enum TextureIndex txIndex
);
*/

#endif
