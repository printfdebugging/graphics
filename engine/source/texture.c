#include <stdio.h>
#include <stdlib.h>

#include "stb_image.h"

#include "engine/texture.h"
#include "engine/core/defines.h"

status texture_load_from_file(struct texture *texture, const char *path) {
	i32 image_width;
	i32 image_height;
	i32 image_channel_count;
	unsigned char *image_data;

	stbi_set_flip_vertically_on_load(true);
	image_data = stbi_load(path, &image_width, &image_height, &image_channel_count, 0);

	// TODO: make this a bit more robust using switch
	i32 image_format;
	if (image_channel_count == 3)
		image_format = GL_RGB;
	else
		image_format = GL_RGBA;

	if (!image_data) {
		fprintf(stderr, "Failed to load texture %s\n", path);
		free(texture);
		stbi_image_free(image_data);
		return status_failure;
	}

	if (texture_load(texture, image_data, image_width, image_height, (GLenum) image_format, GL_UNSIGNED_BYTE, image_format, GL_TRUE))
		return status_failure;

	stbi_image_free(image_data);
	return status_success;
}

struct texture *texture_create() {
	struct texture *texture = calloc(1, sizeof(struct texture));
	if (!texture) {
		fprintf(stderr, "Failed to allocate memory for texture\n");
		return NULL;
	}

	*texture = (struct texture) { 0 };
	return texture;
}

struct texture *texture_create_from_file(const char *path) {
	struct texture *texture = texture_create();
	if (!texture)
		return NULL;
	if (texture_load_from_file(texture, path)) {
		texture_destroy(texture);
		return NULL;
	}

	return texture;
}

status texture_load(struct texture *texture, void *data, i32 width, i32 height, GLenum format, GLenum data_type, i32 internal_format, b8 generate_mipmaps) {
	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glTexImage2D(GL_TEXTURE_2D, GL_ZERO, internal_format, width, height, GL_ZERO, format, data_type, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (generate_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	return status_success;
}

void texture_destroy(struct texture *texture) {
	glDeleteTextures(1, &texture->texture);
	free(texture);
}
