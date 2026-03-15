#include "camera.h"
#include "GLFW/glfw3.h"

struct camera *camera_create() {
    struct camera *camera = malloc(sizeof(struct camera));
    if (!camera) {
        fprintf(stderr, "failed to allocate memory for camera\n");
        return NULL;
    }

    camera->position = (vec3s) { 5.0f, 5.0f, 5.0f };
    camera->front = (vec3s) { 0.0f, 0.0f, -5.0f };
    camera->up = (vec3s) { 0.0f, 1.0f, 0.0f };
    camera->yaw = -135.0f;
    camera->pitch = -38.0f;
    camera->x = 400;
    camera->y = 300;
    camera->fov = 45.0f;
    camera->movement_speed = 2.5f;
    camera->mouse_sensitivity = 0.1f;

    return camera;
}

void camera_process_keyboard(struct camera *camera, enum camera_direction direction, float delta_time) {
    const float camera_speed = camera->movement_speed * delta_time;

    switch (direction) {
        case CAMERA_DIRECTION_FORWARD: {
            vec3s mul = glms_vec3_scale(camera->front, camera_speed);
            camera->position = glms_vec3_add(camera->position, mul);
            break;
        }
        case CAMERA_DIRECTION_BACKWARD: {
            vec3s mul = glms_vec3_scale(camera->front, camera_speed);
            camera->position = glms_vec3_sub(camera->position, mul);
            break;
        }
        case CAMERA_DIRECTION_LEFT: {
            vec3s cross = glms_cross(camera->front, camera->up);
            cross = glms_normalize(cross);
            vec3s mul = glms_vec3_scale(cross, camera_speed);
            camera->position = glms_vec3_sub(camera->position, mul);
            break;
        }
        case CAMERA_DIRECTION_RIGHT: {
            vec3s cross = glms_cross(camera->front, camera->up);
            cross = glms_normalize(cross);
            vec3s mul = glms_vec3_scale(cross, camera_speed);
            camera->position = glms_vec3_add(camera->position, mul);
            break;
        }

        default:
            fprintf(stderr, "wrong CameraDirection\n");
            break;
    }
}

static void camera_calculate_direction(struct camera *camera) {
    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3s direction = {
        .x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        .y = sin(glm_rad(camera->pitch)),
        .z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };

    direction = glms_normalize(direction);
    camera->front = direction;
}

static bool left_button_was_pressed = false;

void camera_process_mouse_movement(struct camera *camera, float x, float y, bool left_button_pressed) {
    if (!left_button_pressed) {
        left_button_was_pressed = false;
        camera_calculate_direction(camera);
        return;
    }

    if (!left_button_was_pressed && left_button_pressed) {
        camera->x = x;
        camera->y = y;
        left_button_was_pressed = true;
        return;
    }

    float xoffset = camera->x - x;
    float yoffset = camera->y - y;
    xoffset *= camera->mouse_sensitivity;
    yoffset *= camera->mouse_sensitivity;

    camera->x = x;
    camera->y = y;
    camera->yaw += xoffset;
    camera->pitch -= yoffset;

    camera_calculate_direction(camera);
}

void camera_process_mouse_scroll(struct camera *camera, float yoffset) {
    camera->fov -= (float) yoffset;
    if (camera->fov < 1.0)
        camera->fov = 1.0f;
    if (camera->fov > 45.0f)
        camera->fov = 45.0f;
}

mat4s camera_get_view_matrix(struct camera *camera) {
    vec3s sum = glms_vec3_add(camera->position, camera->front);
    return glms_lookat(camera->position, sum, camera->up);
}

void camera_destroy(struct camera *camera) {
    free(camera);
}
