#include <stdlib.h>
#include <stdio.h>

#include "glad/glad.h"
#include "cglm/struct.h"
#include "GLFW/glfw3.h"

#include "engine/shader.h"
#include "engine/font/font.h"
#include "engine/core/string.h"

#include "editor/font/renderer.h"

/* todo: think about the api/organization/malloc/free later,
 * first we need to have some font rendered on the screen, design
 * and organization comes after that. */
status font_renderer_init(struct font_renderer *renderer) {
	renderer->vertices = NULL;
	renderer->vertices_count = 0;

	glGenVertexArrays(1, &renderer->vertex_array_object);
	glGenBuffers(1, &renderer->vertex_buffer_object);

	/* note: we would want to create a buffer with some pre-defined
	 * size in case we are constantly changing the text to render, in
	 * a text editor for example.
	 *
	 * note: in other cases, we might want to keep it fixed size like
	 * in a toolkit for static labels, since they don't change ever.
	 */

	renderer->initialized = true;
	renderer->uploaded = false;
	return status_success;
}

status font_renderer_destroy(struct font_renderer *renderer) {
	if (renderer->vertices)
		free(renderer->vertices);

	glDeleteBuffers(1, &renderer->vertex_buffer_object);
	glDeleteVertexArrays(1, &renderer->vertex_array_object);

	return 0;
}

status font_renderer_load_text(struct font_renderer *renderer, struct font *font, const char *text) {
	hb_buffer_t *buffer = hb_buffer_create();
	/* note: todo: the text data structure should have these properties. */
	/* note: think about parallelising this later when you have an editor and it's data structures up and running and some basic rendering going on */
	hb_buffer_add_utf8(buffer, text, -1, 0, -1);
	hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	hb_buffer_set_language(buffer, hb_language_from_string("en", -1));
	/* todo: enable ligatures and see how they are rendered */

	hb_shape(font->font, buffer, NULL, 0);

	u32 glyph_count;
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
	hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer, &glyph_count);

	renderer->vertices = calloc(glyph_count * 6, sizeof(struct glyph_vertex));
	renderer->vertices_count = glyph_count * 6;
	/* note: we send 6 vertices per glyph i.e. 2 triangles */
	if (!renderer->vertices) {
		fprintf(stderr, "failed to allocate renderer->vertices\n");
		return status_failure;
	}

	/* todo: this should be abstracted away as rows */
	/* todo: separate editor renderer from the engine renderer,
	 * - here renderer should provide the base functionality
	 * - editor should build abstractions/wrappers around that */
	struct point position = { .x = 0, .y = 400 };
	status rc = status_success;
	for (u32 glyph_index = 0; glyph_index < glyph_count; ++glyph_index) {
		hb_codepoint_t glyphid = glyph_info[glyph_index].codepoint;
		struct glyph_info info;
		if (!(rc = font_lookup_glyph(font, (u16) glyphid, &info))) {
			fprintf(stderr, "failed to lookup glyph at index: %i\n", glyphid);
			continue;
		}

		f64 fontsize = 30;
		f64 scale = fontsize / info.upem;
		// todo: look into what this is useful for struct extents ink_extents;
		position.x += scale * glyph_positions[glyph_index].x_offset;
		position.y += scale * glyph_positions[glyph_index].y_offset;

		if (info.empty) {
			fprintf(stderr, "glyph info empty for glyph id: %i", glyphid);
			continue;
		}

		struct glyph_vertex corners[4];
		for (int corner_index = 0; corner_index < 4; corner_index++) {
			int cx = (corner_index >> 1) & 1;
			int cy = corner_index & 1;

			double ex = (1 - cx) * info.extents.min_x + cx * info.extents.max_x;
			double ey = (1 - cy) * info.extents.min_y + cy * info.extents.max_y;

			corners[corner_index] = (struct glyph_vertex) {
				.x = (f32) (position.x + scale * ex),
				.y = (f32) (position.y + scale * ey),
				.tx = (f32) ex,
				.ty = (f32) ey,
				.nx = cx ? 1.f : -1.f,
				.ny = cy ? -1.f : 1.f,
				.emPerPos = (float) (1.0 / scale),
				.atlas_offset = info.atlas_offset / TEXEL_SIZE,
				.atlas_page = info.atlas_page,
			};
		}

		u32 index = glyph_index * 6;

		renderer->vertices[index + 0] = corners[0];
		renderer->vertices[index + 1] = corners[1];
		renderer->vertices[index + 2] = corners[2];
		renderer->vertices[index + 3] = corners[1];
		renderer->vertices[index + 4] = corners[2];
		renderer->vertices[index + 5] = corners[3];

		position.x += scale * glyph_positions[glyph_index].x_advance;
		position.y += scale * glyph_positions[glyph_index].y_advance;
	}

	return status_success;
}

/* this would be used for chunks at a time, so the renderer would have to rebuild renderer options from the layouting layer */
status font_renderer_render_text(struct font_renderer *renderer, struct font *font, struct shader *shader, struct font_renderer_options renderer_opts) {
	if (!renderer->uploaded || !renderer->initialized) {
		fprintf(stderr, "renderer either not initialized or data not uploaded to gpu\n");
	}

	b8 debug_enabled = false;
	b8 stem_darkening_enabled = true;

	struct atlas_page *current_page = &font->atlas->pages[font->atlas->used_pages_count - 1];
	u32 current_texture = current_page->texture;
	u32 current_texture_unit = current_page->texture_unit;

	glBindVertexArray(renderer->vertex_array_object);
	shader_use(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader->program, "u_matViewProjection"), 1, GL_FALSE, renderer_opts.transformation_matrix.col[0].raw);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glUniform2f(glGetUniformLocation(shader->program, "u_viewport"), (float) viewport[2], (float) viewport[3]);

	/* todo: cache the shader variable locations here */
	glUniform1f(glGetUniformLocation(shader->program, "u_scale"), renderer_opts.font_size);

	int location = glGetUniformLocation(shader->program, "hb_gpu_atlas");
	glUniform1i(location, (i32) current_texture_unit - GL_TEXTURE0);

	location = glGetUniformLocation(shader->program, "u_gamma");
	if (location == -1) {
		fprintf(stderr, "uniform for %s not found\n", "u_gamma");
	}
	glUniform1f(location, 1.0);

	location = glGetUniformLocation(shader->program, "u_foreground");
	glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.f);
	if (location == -1) {
		fprintf(stderr, "uniform for %s not found\n", "u_foreground");
	}

	location = glGetUniformLocation(shader->program, "u_debug");
	glUniform1f(location, debug_enabled ? 1.f : 0.f);
	if (location == -1) {
		fprintf(stderr, "uniform for %s not found\n", "u_debug");
	}

	location = glGetUniformLocation(shader->program, "u_stem_darkening");
	glUniform1f(location, stem_darkening_enabled ? 1.f : 0.f);
	if (location == -1) {
		fprintf(stderr, "uniform for %s not found\n", "u_stem-darkening");
	}

	glDrawArrays(GL_TRIANGLES, 0, (i32) renderer->vertices_count);
	return 0;
}

void font_renderer_setup_quad_locations(struct font_renderer *renderer, struct shader *shader) {
	if (shader == NULL) {
		fprintf(stderr, "shader not prepared yet, try again\n");
		return;
	}

	u32 program = shader->program;

	GLsizei stride = sizeof(struct glyph_vertex);

	i32 location;

	location = glGetAttribLocation(program, "a_position");
	glEnableVertexAttribArray((u32) location);
	glVertexAttribPointer((u32) location, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(struct glyph_vertex, x));

	location = glGetAttribLocation(program, "a_texcoord");
	glEnableVertexAttribArray((u32) location);
	glVertexAttribPointer((u32) location, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(struct glyph_vertex, tx));

	location = glGetAttribLocation(program, "a_normal");
	glEnableVertexAttribArray((u32) location);
	glVertexAttribPointer((u32) location, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(struct glyph_vertex, nx));

	location = glGetAttribLocation(program, "a_emPerPos");
	glEnableVertexAttribArray((u32) location);
	glVertexAttribPointer((u32) location, 1, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(struct glyph_vertex, emPerPos));

	location = glGetAttribLocation(program, "a_glyphLoc");
	glEnableVertexAttribArray((u32) location);
	glVertexAttribIPointer((u32) location, 1, GL_UNSIGNED_INT, stride, (const void *) offsetof(struct glyph_vertex, atlas_offset));

	/* todo: let's let them bound for now, we are debugging why things don't work */
	/* it was these KO!!!!!! i should not unbind unnecessarily. */
	// glBindVertexArray(0);
	// glUseProgram(0);
}

void font_renderer_upload_to_gpu(struct font_renderer *renderer) {
	if (renderer->uploaded) {
		fprintf(stderr, "renderer data already uploaded\n");
		return;
	}

	if (!renderer->initialized) {
		fprintf(stderr, "renderer not initialized\n");
		return;
	}

	glBindVertexArray(renderer->vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct glyph_vertex) * renderer->vertices_count, renderer->vertices, GL_STATIC_DRAW);
	renderer->uploaded = true;
}

void glyph_vertex_print_info(struct glyph_vertex *vertex, FILE *descriptor) {
	char buf[1024];

	const char *glyph_vertex_info_string =
	    "glyph_vertex {\n"
	    "   .x            = %f\n"
	    "   .y            = %f\n"
	    "   .tx           = %f\n"
	    "   .ty           = %f\n"
	    "   .nx           = %f\n"
	    "   .ny           = %f\n"
	    "   .emPerPos     = %f\n"
	    "   .atlas_offset = %i\n"
	    "   .atlas_page   = %i\n"
	    "};\n";

	/* clang-format off */
	snprintf(buf, sizeof(buf), glyph_vertex_info_string,
			vertex->x,
			vertex->y,
			vertex->tx,
			vertex->ty,
			vertex->nx,
			vertex->ny,
			vertex->emPerPos,
			vertex->atlas_offset,
			vertex->atlas_page
	);
	/* clang-format on */

	fprintf(descriptor, "%s\n", buf);
}
