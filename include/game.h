#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "camera.h"

struct GameData {
    struct Window *window;
    struct Camera *camera;

    float lastFrame;
    float deltaTime;
};

int gameRun();

#endif
