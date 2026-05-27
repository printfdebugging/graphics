#ifndef GAME_H
#define GAME_H

#include "cglm/struct.h"
#include "camera.h"
#include "window.h"

/* this is the game data which is passed around in the callbacks. this is  not
 * a place to store textures etc, they live separately, though they should be
 * accessible from all other subsystems wherever they are needed, or maybe in
 * the game code, who knows we might do some cool things with them. */
struct game_data {
        struct window *window;
        struct camera *camera;

        float last_frame;
        float delta_time;
        vec3s light_position;
};

int game_run();

#endif
