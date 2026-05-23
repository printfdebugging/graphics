#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "camera.h"

/* this is the game data which is passed around in the callbacks. this is  not
 * a place to store textures etc, they live separately, though they should be
 * accessible from all other subsystems wherever they are needed, or maybe in the
 * game code, who knows we might do some cool things with them. */
struct GameData {
        struct Window *window;
        struct Camera *camera;

        float lastFrame;
        float deltaTime;
};

int gameRun();

#endif
