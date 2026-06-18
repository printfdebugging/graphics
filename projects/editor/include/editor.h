#ifndef EDITOR_H
#define EDITOR_H

#include "cglm/struct.h"

#include "engine/engine.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/font/font.h"
#include "engine/font/renderer.h"
#include "engine/core/defines.h"

struct editor_state {
	/**The bridge connects the engine subsystems with the editor's event handlers. */
	struct bridge bridge;

	struct window *window;
	struct font *font;
	struct font_renderer *font_renderer;
	struct shader *font_shader;

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
