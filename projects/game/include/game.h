#ifndef GAME_H
#define GAME_H

#include "cglm/struct.h"

#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/window.h"
#include "engine/model/model.h"
#include "engine/core/defines.h"

struct game_state {
	/**The bridge connects the engine subsystems with the game's event handlers. */
	struct bridge bridge;

	struct camera *camera;
	struct window *window;

	f64 last_frame;
	f64 delta_time;
	vec3s light_position;

	/* todo: use systems/resource-managers instead of these. */
	struct primitive *axes;
	struct model *cengine;
	struct shader *model_shader;
};

/* */
status game_initialize(struct game_state *game, int argc, char *argv[]);

/* */
status game_run(struct game_state *game);
/*  */
status game_shutdown(struct game_state *game);

#endif
