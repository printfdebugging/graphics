#ifndef GAME_H
#define GAME_H

#include "cglm/struct.h"

#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/window.h"
#include "engine/model.h"
#include "engine/core/defines.h"

/* this is the game data which is passed around in the callbacks. this is  not
 * a place to store textures etc, they live separately, though they should be
 * accessible from all other subsystems wherever they are needed, or maybe in
 * the game code, who knows we might do some cool things with them. */
struct game_state {
        /* todo: rename engine_state to engine_interface or something like that where all that contains is funciton pointers to callbacks */
        /* engine here is just to get us the events  in the game code, we handle them here */
        struct app_bridge bridge;
        struct camera *camera;
        struct window *window;

        f64 last_frame;
        f64 delta_time;
        vec3s light_position;

        /* todo: putting them here for now, will remove them later as i have some resource containers */
        struct primitive *axes;
        struct model *cengine;
};

i8 game_initialize(struct game_state *game, int argc, char *argv[]);
i8 game_run(struct game_state *game);
i8 game_shutdown(struct game_state *game);

#endif
