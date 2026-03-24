#include "texture.h"
#include "logger.h"
#include "shader.h"

#include "stb_image.h"

int texture_load_from_file(struct texture *texture, const char *path) {
    int image_width;
    int image_height;
    int image_channel_count;
    unsigned char *image_data;

    stbi_set_flip_vertically_on_load(true);
    image_data = stbi_load(path, &image_width, &image_height, &image_channel_count, 0);

    // TODO: make this a bit more robust using switch
    GLenum imgFormat;
    if (image_channel_count == 3)
        imgFormat = GL_RGB;
    else
        imgFormat = GL_RGBA;

    if (!image_data) {
        fprintf(stderr, "Failed to load texture %s\n", path);
        free(texture);
        stbi_image_free(image_data);
        return 1;
    }

    if (texture_load(texture, image_data, image_width, image_height, imgFormat, GL_UNSIGNED_BYTE, imgFormat, GL_TRUE))
        return 1;

    stbi_image_free(image_data);
    return 0;
}

struct texture *texture_create() {
    struct texture *texture = malloc(sizeof(struct texture));
    if (!texture) {
        fprintf(stderr, "Failed to allocate memory for texture\n");
        return NULL;
    }

    *texture = (struct texture) { 0 };
    return texture;
}

int texture_load(struct texture *texture, void *data, unsigned int width, unsigned int height, GLenum format, GLenum data_type, GLenum internal_format, GLboolean generate_mipmaps) {
    glGenTextures(1, &texture->texture);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
    glTexImage2D(GL_TEXTURE_2D, GL_ZERO, internal_format, width, height, GL_ZERO, format, data_type, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (generate_mipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    return 0;
}

void texture_destroy(struct texture *texture) {
    glDeleteTextures(1, &texture->texture);
    free(texture);
}
