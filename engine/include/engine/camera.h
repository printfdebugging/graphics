#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>

#include "cglm/struct.h"

// by any chance can this clash with external project's core/defines.h? let's try that
#include "engine/core/defines.h"

enum camera_direction {
        CAMERA_DIRECTION_FORWARD = 0,
        CAMERA_DIRECTION_BACKWARD = 1,
        CAMERA_DIRECTION_LEFT = 2,
        CAMERA_DIRECTION_RIGHT = 3
};

#define CAMERA_DEFAULTS                               \
        .position = (vec3s) { { 0.0f, 1.0f, 6.0f } }, \
        .front = (vec3s) { { 0.0f, 0.0f, -1.0f } },   \
        .up = (vec3s) { { 0.0f, 1.0f, 0.0f } },       \
        .yaw = -90.0f,                                \
        .pitch = 0.0f,                                \
        .x = 400,                                     \
        .y = 300,                                     \
        .fov = 45.0f,                                 \
        .movement_speed = 17.5f,                      \
        .mouse_sensitivity = 0.1f

struct camera {
        vec3s position;
        vec3s front;
        vec3s up;
        f32 yaw;
        f32 pitch;
        /* what is camera.x and camera.y ?
         * i should document these*/
        f32 x;
        f32 y;
        f32 fov;
        f32 movement_speed;
        f32 mouse_sensitivity;
};

i8 camera_create(struct camera **camera, struct camera opts);
void camera_process_keyboard(struct camera *camera, enum camera_direction direction, f64 delta_time);
void camera_process_mouse_movement(struct camera *camera, f32 x, f32 y, b8 left_button_pressed);
void camera_process_mouse_scroll(struct camera *camera, f32 yoffset);
void camera_adjust_direction(struct camera *camera);
mat4s camera_get_view_matrix(struct camera *camera);
void camera_destroy(struct camera *camera);
void camera_print_info(struct camera *camera, FILE *descriptor);

#endif
