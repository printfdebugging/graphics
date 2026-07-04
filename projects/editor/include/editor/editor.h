#ifndef EDITOR_H
#define EDITOR_H

#include "cglm/struct.h"

#include "engine/engine.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/font/font.h"
#include "engine/core/defines.h"

#include "editor/font/renderer.h"

/* todo: later we will most likely separate layouting from the actual text
 * data structure (ropes), but for now we keep it like this just to get
 * things started. */
struct editor_row {
	char *raw_data;
	u32 raw_data_size;

	struct font_renderer renderer_data;
};

struct editor_state {
	/**The bridge connects the engine subsystems with the editor's event handlers. */
	struct bridge bridge;

	struct window *window;
	struct font *font;
	struct shader *font_shader;

	/* text rendering specific */
	const char *font_filepath;
	f32 font_size;

	/* Number of `rows` we can draw on the window.
	 * Each row is `font_size` pixels high. */
	u32 screen_rows;

	struct editor_row *rows;
	u32 rows_count;
	u32 row_offset;

	f64 last_frame;
	f64 delta_time;
};

/* */
status editor_initialize(struct editor_state *editor, int argc, char *argv[]);

/* */
status editor_run(struct editor_state *editor);
/*  */
status editor_shutdown(struct editor_state *editor);

#endif
