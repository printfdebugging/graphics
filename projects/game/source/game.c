#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/primitive.h"
#include "engine/model.h"
#include "engine/renderer.h"
#include "engine/shader.h"
#include "engine/window.h"
#include "engine/core/defines.h"

#include "game.h"

void mouse_move(void *userdata, struct window *window, f64 x, f64 y);
void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y);
void window_resize(void *userdata, struct window *window, i32 w, i32 h);

/* this is for keyboard inputs which we don't get via callbacks, but we check for
 * explicitly in the main loop maybe there's another way, let's ask nitrix later */
void process_input(struct game_state *game, struct window *window, f64 delta_time);

struct primitive *create_axes_primitive();
struct shader *create_axes_shader();

i8 game_initialize(struct game_state *game, int argc, char *argv[]) {
	i8 status = 0;
	(void) argc;
	(void) argv;

	game->bridge = (struct bridge) {
		.mouse_move = mouse_move,
		.mouse_scroll = mouse_scroll,
		.window_resize = window_resize,
	};

	struct window window_options = {
		WINDOW_DEFAULTS,
		.bridge = &game->bridge,
	};

	if ((status = window_create(&game->window, window_options)) != 0) {
		/* note: we expect the error site to print the error message, so we don't do it in between */
		/* note: only the endpoints print error messages, rest all log them */
		return status;
	}

	struct camera camera_options = {
		CAMERA_DEFAULTS,
		.position = { { 0.0f, 4.0f, 9.0f } },
		.movement_speed = 5.0f,
		.pitch = -30.0f,
	};

	if ((status = camera_create(&game->camera, camera_options)) != 0) {
		return status;
	}

	game->light_position = (vec3s) { { 0.0f, 0.0f, 2.0f } };
	camera_adjust_direction(game->camera);

	/* todo: add primitves etc to the game, or atleast some resource arenas */
	/* todo: the shaders etc live in the model struct, nested deep to the level of primitives. so we never create them manually. */
	/* todo: have some kind of resource specifier file which links a model to it's respective shaders, ideally a header file in the code, makes things much easier, something like `resource.h` */
	/* todo: continue here, we were moving old main to this new game/engine layout */

	game->axes = create_axes_primitive();
	struct shader *axes_shader = create_axes_shader();
	if (!game->axes || !axes_shader)
		return EXIT_FAILURE;

	game->axes->shader = axes_shader;

	/* todo: create a proper shader for gltf models with a lot of optional features (inside include guards) */
	struct shader *engine_shader = shader_create();
	if ((status = shader_load_from_file(engine_shader, ASSETS_DIR "shaders/gltf/shader.vert", ASSETS_DIR "shaders/gltf/shader.frag")) != 0) {
		fprintf(stderr, "failed to load shader\n");
		return EXIT_FAILURE;
	}

	const char *engine_asset_path = ASSETS_DIR "models/CylinderEngine/glTF/2CylinderEngine.gltf";
	game->cengine = malloc(sizeof(struct model));
	if (!game->cengine) {
		fprintf(stderr, "failed to allocate model\n");
		return EXIT_FAILURE;
	}

	model_init(game->cengine);
	/* todo: decouple this, passing this here for now, for testing purposes */
	if ((status = model_load_from_file(game->cengine, engine_asset_path, engine_shader)) != 0) {
		fprintf(stderr, "failed to load model: %s\n", engine_asset_path);
		return EXIT_FAILURE;
	}

	return status;
}

i8 game_run(struct game_state *game) {
	i8 status = 0;
	while (!window_close(game->window)) {
		f64 current_frame = glfwGetTime();
		game->delta_time = current_frame - game->last_frame;
		game->last_frame = current_frame;

		window_poll_events(game->window);
		window_process_input(game->window);
		window_clear_color(game->window);
		process_input(game, game->window, game->delta_time);

		mat4s model = { GLM_MAT4_IDENTITY_INIT };
		mat4s view = camera_get_view_matrix(game->camera);
		mat4s projection = glms_perspective(glm_rad(game->camera->fov), (float) game->window->width / (float) game->window->height, 0.1f, 100.0f);
		struct transform transform = { .model = model, .view = view, .projection = projection };
		transform.model = glms_scale(transform.model, (vec3s) { { 0.05, 0.05, 0.05 } });

		render_primitive(game->axes, transform);
		game->cengine->transform = transform;
		render_model(game->cengine);
		window_swap_buffers(game->window);
	}

	return status;
}

i8 game_shutdown(struct game_state *game) {
	i8 status = 0;

	primitive_destroy(game->axes);
	/* todo: do this till we have a shader_manager or something similar which can take care of the lifetimes responsibly */
	shader_destroy((*game->cengine->primitives)->shader);
	model_destroy(game->cengine);

	camera_destroy(game->camera);
	window_destroy(game->window);
	/* todo: no need to destroy the game state itself, it's allocated on the stack in main */

	return status;
}

void input_move_point_light(struct window *window, f64 delta_time) {
	(void) delta_time;
	struct game_state *data = glfwGetWindowUserPointer(window->window);
	if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
		data->light_position.z -= 0.1f;
	if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
		data->light_position.z += 0.1f;
	if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
		data->light_position.x -= 0.1f;
	if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
		data->light_position.x += 0.1f;
}

void input_move_camera(struct game_state *game, struct window *window, f64 delta_time) {
	/* todo: need to fix this game_state passing around through callbacks to get things going
	 * todo: write a good commit message explaining all these, the arcitecture etc. */
	if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
		camera_process_keyboard(game->camera, CAMERA_DIRECTION_FORWARD, delta_time);
	if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
		camera_process_keyboard(game->camera, CAMERA_DIRECTION_BACKWARD, delta_time);
	if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
		camera_process_keyboard(game->camera, CAMERA_DIRECTION_LEFT, delta_time);
	if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
		camera_process_keyboard(game->camera, CAMERA_DIRECTION_RIGHT, delta_time);
}

void process_input(struct game_state *game, struct window *window, f64 delta_time) {
	// input_move_point_light(window, delta_time);
	input_move_camera(game, window, delta_time);
}

void mouse_move(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	struct game_state *game = userdata;
	camera_process_mouse_movement(game->camera, (f32) x, (f32) y, glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	(void) window;
	struct game_state *game = userdata;
	camera_process_mouse_scroll(game->camera, (float) y);
}

void window_resize(void *userdata, struct window *window, i32 w, i32 h) {
	(void) userdata;
	(void) window;
	(void) w;
	(void) h;
	fprintf(stderr, "window resized!!!\n");
}

struct primitive *create_axes_primitive() {
	const i32 AXES = 2;
	const i32 LINES_PER_AXIS = 11;
	const i32 LINES_ON_EACH_SIDE = LINES_PER_AXIS / 2;
	const i32 POINTS_PER_LINE = 2;
	const i32 FLOATS_PER_POINT = 3;
	const i32 count = AXES * LINES_PER_AXIS * POINTS_PER_LINE;

	f32 vertices[AXES][LINES_PER_AXIS][POINTS_PER_LINE][FLOATS_PER_POINT];

	for (i32 z = -LINES_ON_EACH_SIDE; z <= LINES_ON_EACH_SIDE; ++z) {
		vertices[0][z + LINES_ON_EACH_SIDE][0][0] = (f32) -LINES_ON_EACH_SIDE;
		vertices[0][z + LINES_ON_EACH_SIDE][0][1] = 0.0f;
		vertices[0][z + LINES_ON_EACH_SIDE][0][2] = (f32) z;

		vertices[0][z + LINES_ON_EACH_SIDE][1][0] = (f32) LINES_ON_EACH_SIDE;
		vertices[0][z + LINES_ON_EACH_SIDE][1][1] = 0.0f;
		vertices[0][z + LINES_ON_EACH_SIDE][1][2] = (f32) z;
	}

	for (i32 x = -LINES_ON_EACH_SIDE; x <= LINES_ON_EACH_SIDE; ++x) {
		vertices[1][x + LINES_ON_EACH_SIDE][0][0] = (f32) x;
		vertices[1][x + LINES_ON_EACH_SIDE][0][1] = 0.0f;
		vertices[1][x + LINES_ON_EACH_SIDE][0][2] = (f32) -LINES_ON_EACH_SIDE;

		vertices[1][x + LINES_ON_EACH_SIDE][1][0] = (f32) x;
		vertices[1][x + LINES_ON_EACH_SIDE][1][1] = 0.0f;
		vertices[1][x + LINES_ON_EACH_SIDE][1][2] = (f32) LINES_ON_EACH_SIDE;
	}

	struct primitive *axes = malloc(sizeof(struct primitive));
	if (!axes)
		return NULL;
	primitive_init(axes);
	primitive_create_vertex_array(axes);
	primitive_load_vertices(axes, &vertices[0][0][0][0], (u32) count, 3 * sizeof(float));
	axes->draw_mode = GL_LINES;
	return axes;
}

struct shader *create_axes_shader() {
	struct shader *axes_shader = shader_create();
	if (!axes_shader)
		return NULL;
	if (shader_load_from_file(axes_shader, ASSETS_DIR "shaders/lines/shader.vert", ASSETS_DIR "shaders/lines/shader.frag")) {
		shader_destroy(axes_shader);
		return NULL;
	}
	return axes_shader;
}
