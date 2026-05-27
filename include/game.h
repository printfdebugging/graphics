#ifndef GAME_H
#define GAME_H

#include "cglm/struct.h"

#include "camera.h"
#include "window.h"
#include "core/defines.h"

/* this is the game data which is passed around in the callbacks. this is  not
 * a place to store textures etc, they live separately, though they should be
 * accessible from all other subsystems wherever they are needed, or maybe in
 * the game code, who knows we might do some cool things with them. */
struct game_data {
        struct window *window;
        struct camera *camera;

        f64 last_frame;
        f64 delta_time;
        vec3s light_position;
};

i8 game_run();

#endif
