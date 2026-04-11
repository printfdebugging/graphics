#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "camera.h"

struct game_data {
    struct window *window;
    struct camera *camera;

    float last_frame;
    float delta_time;
};

int game_run();

#endif
