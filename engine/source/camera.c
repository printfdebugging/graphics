#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cglm/struct.h"

#include "engine/camera.h"
#include "engine/core/defines.h"

i8 camera_create(struct camera **camera, struct camera opts) {
	*camera = malloc(sizeof(struct camera));
	if (!*camera) {
		fprintf(stderr, "failed to allocate memory for camera\n");
		return 1;
	}

	**camera = opts;
	return 0;
}

void camera_process_keyboard(struct camera *camera, enum camera_direction direction, f64 delta_time) {
	const f32 cameraSpeed = camera->movement_speed * (f32) delta_time;

	switch (direction) {
		case CAMERA_DIRECTION_FORWARD: {
			vec3s mul = glms_vec3_scale(camera->front, cameraSpeed);
			camera->position = glms_vec3_add(camera->position, mul);
			break;
		}
		case CAMERA_DIRECTION_BACKWARD: {
			vec3s mul = glms_vec3_scale(camera->front, cameraSpeed);
			camera->position = glms_vec3_sub(camera->position, mul);
			break;
		}
		case CAMERA_DIRECTION_LEFT: {
			vec3s cross = glms_cross(camera->front, camera->up);
			cross = glms_normalize(cross);
			vec3s mul = glms_vec3_scale(cross, cameraSpeed);
			camera->position = glms_vec3_sub(camera->position, mul);
			break;
		}
		case CAMERA_DIRECTION_RIGHT: {
			vec3s cross = glms_cross(camera->front, camera->up);
			cross = glms_normalize(cross);
			vec3s mul = glms_vec3_scale(cross, cameraSpeed);
			camera->position = glms_vec3_add(camera->position, mul);
			break;
		}
		default:
			fprintf(stderr, "wrong CameraDirection\n");
			break;
	}
}

void camera_adjust_direction(struct camera *camera) {
	if (camera->pitch > 89.0f)
		camera->pitch = 89.0f;
	if (camera->pitch < -89.0f)
		camera->pitch = -89.0f;

	vec3s direction = {
		.x = (f32) (cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch))),
		.y = (f32) (sin(glm_rad(camera->pitch))),
		.z = (f32) (sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch))),
	};

	direction = glms_normalize(direction);
	camera->front = direction;
}

static b8 left_button_was_pressed = false;

void camera_process_mouse_movement(struct camera *camera, f32 x, f32 y, b8 left_button_pressed) {
	if (!left_button_pressed) {
		left_button_was_pressed = false;
		return;
	}

	if (!left_button_was_pressed && left_button_pressed) {
		camera->x = x;
		camera->y = y;
		left_button_was_pressed = true;
		return;
	}

	f32 xoffset = camera->x - x;
	f32 yoffset = camera->y - y;
	xoffset *= camera->mouse_sensitivity;
	yoffset *= camera->mouse_sensitivity;

	camera->x = x;
	camera->y = y;
	camera->yaw += xoffset;
	camera->pitch -= yoffset;

	camera_adjust_direction(camera);
}

void camera_process_mouse_scroll(struct camera *camera, f32 yoffset) {
	camera->fov -= (f32) yoffset;
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

void camera_print_info(struct camera *camera, FILE *descriptor) {
	char buf[1024];

	const char *camera_info_string =
	    "camera {\n"
	    "   .position = { %f, %f, %f }\n"
	    "   .front    = { %f, %f, %f }\n"
	    "   .up       = { %f, %f, %f }\n"
	    "   .yaw      =  %f\n"
	    "   .pitch    =  %f\n"
	    "   .x        =  %f\n"
	    "   .y        =  %f\n"
	    "   .fov      =  %f\n"
	    "   .movement_speed     =  %f\n"
	    "   .mouse_sensitivity  =  %f\n"
	    "};\n";

	/* clang-format off */
        snprintf(buf, sizeof(buf), camera_info_string,
                camera->position.x, camera->position.y, camera->position.z,
                camera->front.x, camera->front.y, camera->front.z,
                camera->up.x, camera->up.y, camera->up.z,
                camera->yaw,
                camera->pitch,
                camera->x,
                camera->y,
                camera->fov,
                camera->movement_speed,
                camera->mouse_sensitivity
        );
	/* clang-format on */

	fprintf(descriptor, "%s\n", buf);
}
