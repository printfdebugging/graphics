#ifndef EDITOR_H
#define EDITOR_H

#include "cglm/struct.h"

#include "engine/engine.h"
#include "engine/window.h"
#include "engine/shader.h"
#include "engine/core/defines.h"

struct editor_state {
	/**The bridge connects the engine subsystems with the editor's event handlers. */
	struct bridge bridge;

	struct window *window;
	struct shader *font_shader;

	f64 last_frame;
	f64 delta_time;
};

/* */
i8 editor_initialize(struct editor_state *editor, int argc, char *argv[]);

/* */
i8 editor_run(struct editor_state *editor);
/*  */
i8 editor_shutdown(struct editor_state *editor);

#endif
