#include <stdio.h>

#include "glad/glad.h"

#include "engine/font/atlas.h"

void atlas_init(struct atlas *atlas) {
	atlas->used_pages_count = 0;
	for (u32 page_index = 0; page_index < MAX_TEXTURE_COUNT; ++page_index) {
		atlas->pages[page_index] = (struct atlas_page) {
			.texture = 0,
			.texture_unit = 0,
			.texture_buffer_object = 0,
			.capacity = 0,
			.cursor = 0,
			.initialized = false,
		};
	}
}

status atlas_create_page(struct atlas *atlas) {
	if (MAX_TEXTURE_COUNT <= atlas->used_pages_count) {
		fprintf(stderr, "atlas has already used all it's pages\n");
		return status_failure;
	}

	atlas->used_pages_count++;

	u32 texture;
	u32 texture_buffer_object;
	u32 texture_unit_index = atlas->used_pages_count - 1;
	u32 texture_unit = GL_TEXTURE0 + texture_unit_index;
	u32 capacity = ATLAS_PAGE_SIZE;
	u32 cursor = 0;
	b32 initialized = true;

	glGenBuffers(1, &texture_buffer_object);
	glBindBuffer(GL_TEXTURE_BUFFER, texture_buffer_object);
	glBufferData(GL_TEXTURE_BUFFER, ATLAS_PAGE_SIZE, NULL, GL_STATIC_DRAW);

	glActiveTexture(texture_unit);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA16I, texture_buffer_object);

	atlas->pages[texture_unit_index] = (struct atlas_page) {
		.texture = texture,
		.texture_unit = texture_unit,
		.texture_buffer_object = texture_buffer_object,
		.capacity = capacity,
		.cursor = cursor,
		.initialized = initialized,
	};

	return status_success;
}

status atlas_upload_glyph(struct atlas *atlas, const char *data, u32 lenbytes, struct glyph_info *info) {
	status rc = status_success;
	u32 current_page_index = atlas->used_pages_count - 1;
	struct atlas_page *current_page = &atlas->pages[current_page_index];

	b8 page_out_of_capacity = (current_page->capacity - current_page->cursor) < lenbytes;
	if (page_out_of_capacity) {
		if (!(rc = atlas_create_page(atlas)))
			return rc;
		current_page_index++;
		current_page = &atlas->pages[current_page_index];
	}

	/* note: not sure what this does internally, todo: look into that */
	glBindBuffer(GL_TEXTURE_BUFFER, current_page->texture_buffer_object);
	glBufferSubData(GL_TEXTURE_BUFFER, current_page->cursor, lenbytes, data);

	info->atlas_page = current_page_index;
	info->atlas_offset = current_page->cursor;
	current_page->cursor += lenbytes;

	return status_success;
}

void atlas_destroy(struct atlas *atlas) {
	for (u32 page_index = 0; page_index < atlas->used_pages_count; ++page_index) {
		u32 texture = atlas->pages[page_index].texture;
		u32 texture_buffer_object = atlas->pages[page_index].texture_buffer_object;
		glDeleteTextures(1, &texture);
		glDeleteBuffers(1, &texture_buffer_object);
	}
}
