#include "GLFW/glfw3.h"

#include "camera.h"

struct Camera *cameraCreate() {
        struct Camera *camera = malloc(sizeof(struct Camera));
        if (!camera) {
                fprintf(stderr, "failed to allocate memory for camera\n");
                return NULL;
        }

        camera->position         = (vec3s) { 10.0f, 10.0f, 10.0f };
        camera->front            = (vec3s) { 0.0f, 0.0f, -5.0f };
        camera->up               = (vec3s) { 0.0f, 1.0f, 0.0f };
        camera->yaw              = -135.0f;
        camera->pitch            = -38.0f;
        camera->x                = 400;
        camera->y                = 300;
        camera->fov              = 45.0f;
        camera->movementSpeed    = 8.5f;
        camera->mouseSensitivity = 0.1f;

        return camera;
}

void cameraProcessKeyboard(struct Camera *camera, enum CameraDirection direction, float deltaTime) {
        const float cameraSpeed = camera->movementSpeed * deltaTime;

        switch (direction) {
                case CAMERA_DIRECTION_FORWARD: {
                        vec3s mul        = glms_vec3_scale(camera->front, cameraSpeed);
                        camera->position = glms_vec3_add(camera->position, mul);
                        break;
                }
                case CAMERA_DIRECTION_BACKWARD: {
                        vec3s mul        = glms_vec3_scale(camera->front, cameraSpeed);
                        camera->position = glms_vec3_sub(camera->position, mul);
                        break;
                }
                case CAMERA_DIRECTION_LEFT: {
                        vec3s cross      = glms_cross(camera->front, camera->up);
                        cross            = glms_normalize(cross);
                        vec3s mul        = glms_vec3_scale(cross, cameraSpeed);
                        camera->position = glms_vec3_sub(camera->position, mul);
                        break;
                }
                case CAMERA_DIRECTION_RIGHT: {
                        vec3s cross      = glms_cross(camera->front, camera->up);
                        cross            = glms_normalize(cross);
                        vec3s mul        = glms_vec3_scale(cross, cameraSpeed);
                        camera->position = glms_vec3_add(camera->position, mul);
                        break;
                }

                default:
                        fprintf(stderr, "wrong CameraDirection\n");
                        break;
        }
}

void cameraAdjustDirection(struct Camera *camera) {
        if (camera->pitch > 89.0f)
                camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
                camera->pitch = -89.0f;

        vec3s direction = {
                .x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
                .y = sin(glm_rad(camera->pitch)),
                .z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        };

        direction     = glms_normalize(direction);
        camera->front = direction;
}

static bool leftButtonWasPressed = false;

void cameraProcessMouseMovement(struct Camera *camera, float x, float y, bool leftButtonPressed) {
        if (!leftButtonPressed) {
                leftButtonWasPressed = false;
                return;
        }

        if (!leftButtonWasPressed && leftButtonPressed) {
                camera->x            = x;
                camera->y            = y;
                leftButtonWasPressed = true;
                return;
        }

        float xoffset = camera->x - x;
        float yoffset = camera->y - y;
        xoffset *= camera->mouseSensitivity;
        yoffset *= camera->mouseSensitivity;

        camera->x = x;
        camera->y = y;
        camera->yaw += xoffset;
        camera->pitch -= yoffset;

        cameraAdjustDirection(camera);
}

void cameraProcessMouseScroll(struct Camera *camera, float yoffset) {
        camera->fov -= (float) yoffset;
        if (camera->fov < 1.0)
                camera->fov = 1.0f;
        if (camera->fov > 45.0f)
                camera->fov = 45.0f;
}

mat4s cameraGetViewMatrix(struct Camera *camera) {
        vec3s sum = glms_vec3_add(camera->position, camera->front);
        return glms_lookat(camera->position, sum, camera->up);
}

void cameraDestroy(struct Camera *camera) {
        free(camera);
}
