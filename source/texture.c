#include "texture.h"
#include "logger.h"
#include "shader.h"

#include "stb_image.h"

int textureLoadFromFile(struct Texture *texture, const char *path) {
        int            imgWidth;
        int            imgHeight;
        int            imgChanCount;
        unsigned char *imgData;

        stbi_set_flip_vertically_on_load(true);
        imgData = stbi_load(path, &imgWidth, &imgHeight, &imgChanCount, 0);

        // TODO: make this a bit more robust using switch
        GLenum imgFormat;
        if (imgChanCount == 3)
                imgFormat = GL_RGB;
        else
                imgFormat = GL_RGBA;

        if (!imgData) {
                fprintf(stderr, "Failed to load texture %s\n", path);
                free(texture);
                stbi_image_free(imgData);
                return 1;
        }

        if (textureLoad(texture, imgData, imgWidth, imgHeight, imgFormat, GL_UNSIGNED_BYTE, imgFormat, GL_TRUE))
                return 1;

        stbi_image_free(imgData);
        return 0;
}

struct Texture *textureCreate() {
        struct Texture *texture = malloc(sizeof(struct Texture));
        if (!texture) {
                fprintf(stderr, "Failed to allocate memory for texture\n");
                return NULL;
        }

        *texture = (struct Texture) { 0 };
        return texture;
}

int textureLoad(struct Texture *texture, void *data, unsigned int width, unsigned int height, GLenum format, GLenum data_type, GLenum internal_format, GLboolean generate_mipmaps) {
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

void textureDestroy(struct Texture *texture) {
        glDeleteTextures(1, &texture->texture);
        free(texture);
}
