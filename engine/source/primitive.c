#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine/primitive.h"
#include "engine/core/defines.h"
#include "engine/shader.h"

static u32 glTypeToSize(GLenum type) {
	switch (type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;
		case GL_SHORT:
			return sizeof(short);
		case GL_UNSIGNED_SHORT:
			return sizeof(unsigned short);
		case GL_UNSIGNED_INT:
			return sizeof(unsigned int);
		case GL_FLOAT:
			return sizeof(float);
		default:
			fprintf(stderr, "error: invalid opengl type\n");
			exit(1);
	}
}

struct primitive *primitive_create() {
	struct primitive *primitive = malloc(sizeof(struct primitive));
	if (!primitive) {
		fprintf(stderr, "failed to allocate memory for primitive\n");
		return NULL;
	}

	*primitive = (struct primitive) { 0 };
	glGenVertexArrays(1, &primitive->vao);
	return primitive;
}

void primitive_destroy(struct primitive *primitive) {
	if (primitive->vbo_vertex)
		glDeleteBuffers(1, &primitive->vbo_vertex);
	if (primitive->vbo_color)
		glDeleteBuffers(1, &primitive->vbo_color);
	if (primitive->vbo_uv)
		glDeleteBuffers(1, &primitive->vbo_uv);
	if (primitive->ebo)
		glDeleteBuffers(1, &primitive->ebo);
	glDeleteVertexArrays(1, &primitive->vao);
	/* not this, we might do double free since primitives in a model often share a shader*/
	// shader_destroy(primitive->shader);
	free(primitive);
}

/*
 * NOTE: if the vertices are packed together, then the stride would be
 *       the size of an individual chunk (before the attribute start repeating).
 *       also note that the last argument to `glVertexAttribPointer` is 0,
 *       that means that we are going to pass the pointer to the first data
 * entry, not the start of the chunk.
 */
void primitive_load_vertices(struct primitive *primitive, void *data, u32 count, i32 stride) {
	glBindVertexArray(primitive->vao);
	glGenBuffers(1, &primitive->vbo_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, primitive->vbo_vertex);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(PRIMITIVE_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(PRIMITIVE_ATTRIBUTE_POSITION);
	primitive->vertex_count = count;
	primitive->vertex_stride = stride;
}

void primitive_load_indices(struct primitive *primitive, void *data, u32 count, GLenum type, i32 stride) {
	glBindVertexArray(primitive->vao);
	glGenBuffers(1, &primitive->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, glTypeToSize(type) * count, data, GL_STATIC_DRAW);
	primitive->index_count = count;
	primitive->index_stride = stride;
	primitive->index_type = type;
}

void primitive_load_colors(struct primitive *primitive, void *data, u32 count, i32 stride) {
	glBindVertexArray(primitive->vao);
	glGenBuffers(1, &primitive->vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, primitive->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(PRIMITIVE_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(PRIMITIVE_ATTRIBUTE_COLOR);
	primitive->color_count = count;
	primitive->color_stride = stride;
}

void primitive_load_uv(struct primitive *primitive, void *data, u32 count, i32 stride) {
	glBindVertexArray(primitive->vao);
	glGenBuffers(1, &primitive->vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, primitive->vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(PRIMITIVE_ATTRIBUTE_UV, 2, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(PRIMITIVE_ATTRIBUTE_UV);
	primitive->uv_count = count;
	primitive->uv_stride = stride;
}

void primitive_load_normals(struct primitive *primitive, void *data, u32 count, i32 stride) {
	glBindVertexArray(primitive->vao);
	glGenBuffers(1, &primitive->vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, primitive->vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(PRIMITIVE_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(PRIMITIVE_ATTRIBUTE_NORMAL);
	primitive->normal_count = count;
	primitive->normal_stride = stride;
}
