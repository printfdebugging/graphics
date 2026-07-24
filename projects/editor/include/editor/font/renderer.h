#ifndef EDITOR_FONT_RENDERER_H
#define EDITOR_FONT_RENDERER_H

#include <stdio.h>

#include "cglm/struct.h"

#include "engine/shader.h"
#include "engine/font/font.h"
#include "engine/core/defines.h"

/* todo: redo the documentation in this and all other modules once
 * the approach matures and things start working. many things will
 * change in these comments for sure. */

struct glyph_vertex {
	f32 x;
	f32 y;
	f32 tx;
	f32 ty;
	f32 nx;
	f32 ny;
	f32 emPerPos;
	u32 atlas_offset;
	u32 atlas_page;
};

/* renderer shader globals */
struct font_renderer_options {
	f32 font_size;
	vec4s font_color;
	f32 position;
	mat4s transformation_matrix;
};

/** The `font_renderer` does not contain any font related state, or
 * text for that matter. It is just an interface to render something
 * and should not have to store or allocate/manage any object which
 * is not centeral to what it does.
 *
 * 	For example, `struct font` doesn't need to be a part of the
 * 	`font_renderer`, there can be multiple fonts. And the `text`
 * 	we are about to render doesn't have to be in here either.
 */
struct font_renderer {
	/* todo: later we would have multiple fonts */
	// struct font *fonts;
	// u32 fonts_count;

	/** This changes as the text changes. For now we are not
	 * allocating it on init, so that we remain flexible for other
	 * use cases mentioned in the init. */
	struct glyph_vertex *vertices;
	u32 vertices_count;

	/* These refer to the vao and the vbo which hold the vertex
	 * primitives like position, texture coordinates etc. These
	 * are created on init. */
	u32 vertex_array_object;
	u32 vertex_buffer_object;

	/* we are not managing this here for now, saves us some
	 * work in the init function.. but we provide a helper to create
	 * the default shader. */
	// `struct shader *shader;`

	/* these shouldn't live inside the renderer, the
	 * renderer should just be "the state required to render something".
	 */
	// struct font *font;
	// const char *text;  // ?? or maybe some other way to store it outside the renderer? probably so.
	b8 initialized;
	/* todo: temporary variable */
	b8 uploaded;
};

status font_renderer_init(struct font_renderer *renderer);
status font_renderer_destroy(struct font_renderer *renderer);
status font_renderer_load_text(struct font_renderer *renderer, struct font *font, const u32 *runes, const u32 runelen);
status font_renderer_render_text(struct font_renderer *renderer, struct font *font, struct shader *shader, struct font_renderer_options renderer_opts);

void glyph_vertex_print_info(struct glyph_vertex *vertex, FILE *descriptor);

void font_renderer_setup_quad_locations(struct font_renderer *renderer, struct shader *shader);
void font_renderer_upload_to_gpu(struct font_renderer *renderer);

#endif
