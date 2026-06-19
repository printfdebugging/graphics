#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/shader.h"
#include "engine/window.h"
#include "engine/model/primitive.h"
#include "engine/model/model.h"
#include "engine/model/renderer.h"
#include "engine/core/string.h"
#include "engine/core/defines.h"

#include "game.h"

static const char *shader_version = "#version 330 core\n";

void mouse_move(void *userdata, struct window *window, f64 x, f64 y);
void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y);
void window_resize(void *userdata, struct window *window, i32 w, i32 h);

/* this is for keyboard inputs which we don't get via callbacks, but we check for
 * explicitly in the main loop maybe there's another way, let's ask nitrix later */
void process_input(struct game_state *game, struct window *window, f64 delta_time);

status axes_primitive_init(struct primitive *axes);
status axes_shader_init(struct shader *shader);
status model_shader_init(struct shader *shader);

status game_initialize(struct game_state *game, int argc, char *argv[]) {
	status rc = status_success;
	(void) argc;
	(void) argv;

	if (!(game->window = calloc(1, sizeof(struct window))) ||
	    !(game->camera = calloc(1, sizeof(struct camera))) ||
	    !(game->axes = calloc(1, sizeof(struct primitive))) ||
	    !(game->axes->shader = calloc(1, sizeof(struct shader))) ||
	    !(game->model_shader = calloc(1, sizeof(struct shader))) ||
	    !(game->cengine = calloc(1, sizeof(struct model)))) {
		fprintf(stderr, "failed to allocate memory for window\n");
		rc = status_failure;
		goto cleanup;
	}

	game->bridge = (struct bridge) {
		.mouse_move = mouse_move,
		.mouse_scroll = mouse_scroll,
		.window_resize = window_resize,
	};

	struct window window_options = {
		WINDOW_DEFAULTS,
		.bridge = &game->bridge,
	};

	struct camera camera_options = {
		CAMERA_DEFAULTS,
		.position = { { 0.0f, 4.0f, 9.0f } },
		.movement_speed = 5.0f,
		.pitch = -30.0f,
	};

	const char *engine_asset_path = ASSETS_DIR "models/CylinderEngine/glTF/2CylinderEngine.gltf";

	if (!(rc = window_init(game->window, window_options)) ||
	    !(rc = camera_init(game->camera, camera_options)) ||
	    !(rc = axes_primitive_init(game->axes)) ||
	    !(rc = axes_shader_init(game->axes->shader)) ||
	    !(rc = model_shader_init(game->model_shader)) ||
	    !(rc = model_init(game->cengine)) ||
	    !(rc = model_load_from_file(game->cengine, engine_asset_path, game->model_shader))) {
		goto cleanup;
	}

	game->light_position = (vec3s) { { 0.0f, 0.0f, 2.0f } };
	camera_adjust_direction(game->camera);
	return rc;

cleanup:
	game_shutdown(game);
	return rc;
}

status game_run(struct game_state *game) {
	status rc = status_success;
	glfwShowWindow(game->window->window);

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
		// transform.model = glms_scale(transform.model, (vec3s) { { 0.05f, 0.05f, 0.05f } });

		render_primitive(game->axes, transform);
		// game->cengine->transform = transform;
		// render_model(game->cengine);
		window_swap_buffers(game->window);
	}

	return rc;
}

status game_shutdown(struct game_state *game) {
	status rc = status_success;

	primitive_destroy(game->axes);
	/* todo: do this till we have a shader_manager or something similar which can take care of the lifetimes responsibly */
	// shader_destroy((*game->cengine->primitives)->shader);
	model_destroy(game->cengine);

	camera_destroy(game->camera);
	window_destroy(game->window);
	/* todo: no need to destroy the game state itself, it's allocated on the stack in main */

	return rc;
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

status axes_primitive_init(struct primitive *axes) {
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

	primitive_init(axes);
	primitive_create_vertex_array(axes);
	primitive_load_vertices(axes, &vertices[0][0][0][0], (u32) count, 3 * sizeof(float));
	axes->draw_mode = GL_LINES;
	return status_success;
}

status axes_shader_init(struct shader *shader) {
	status rc = status_success;
	struct string *vertex_shader_main = NULL;
	struct string *fragment_shader_main = NULL;

	if (!(vertex_shader_main = string_create_from_file(ASSETS_DIR "shaders/lines/shader.vert")) ||
	    !(fragment_shader_main = string_create_from_file(ASSETS_DIR "shaders/lines/shader.frag"))) {
		rc = status_failure;
		goto cleanup;
	}

	const char *vertex_sources[] = {
		shader_version,
		vertex_shader_main->data,
	};

	const char *fragment_sources[] = {
		shader_version,
		fragment_shader_main->data,
	};

	struct shader_options opts = {
		DEFAULT_SHADER_OPTIONS,
		.has_normals = false,
	};

	if (!(rc = shader_init_with_options(shader, opts, shader_category_model)) ||
	    !(rc = shader_load_from_sources(shader, vertex_sources, array_size(vertex_sources), fragment_sources, array_size(fragment_sources)))) {
		goto cleanup;
	}

cleanup:
	string_destroy(vertex_shader_main);
	string_destroy(fragment_shader_main);
	return rc;
}

status model_shader_init(struct shader *shader) {
	status rc = status_success;
	struct string *engine_vertex_shader_main = NULL;
	struct string *engine_fragment_shader_main = NULL;

	if (!(engine_vertex_shader_main = string_create_from_file(ASSETS_DIR "shaders/gltf/shader.vert")) ||
	    !(engine_fragment_shader_main = string_create_from_file(ASSETS_DIR "shaders/gltf/shader.frag"))) {
		rc = status_failure;
		goto cleanup;
	}

	const char *vertex_sources[] = {
		shader_version,
		engine_vertex_shader_main->data,
	};

	const char *fragment_sources[] = {
		shader_version,
		engine_fragment_shader_main->data,
	};

	struct shader_options shader_opts = {
		DEFAULT_SHADER_OPTIONS,
	};

	if (!(rc = shader_init_with_options(shader, shader_opts, shader_category_model)) ||
	    !(rc = shader_load_from_sources(shader, vertex_sources, array_size(vertex_sources), fragment_sources, array_size(fragment_sources)))) {
		goto cleanup;
	}

cleanup:
	string_destroy(engine_vertex_shader_main);
	string_destroy(engine_fragment_shader_main);
	return rc;
}
