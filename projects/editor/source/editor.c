#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/engine.h"
#include "engine/shader.h"
#include "engine/window.h"
#include "engine/core/string.h"
#include "engine/core/defines.h"

#include "editor.h"

static const char *shader_version = "#version 330 core\n";

void mouse_move(void *userdata, struct window *window, f64 x, f64 y);
void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y);
void window_resize(void *userdata, struct window *window, i32 w, i32 h);

/* this is for keyboard inputs which we don't get via callbacks, but we check for
 * explicitly in the main loop maybe there's another way, let's ask nitrix later */
void process_input(struct editor_state *editor, struct window *window, f64 delta_time);

i8 editor_initialize(struct editor_state *editor, int argc, char *argv[]) {
	i8 status = 0;
	(void) argc;
	(void) argv;

	editor->bridge = (struct bridge) {
		.mouse_move = mouse_move,
		.mouse_scroll = mouse_scroll,
		.window_resize = window_resize,
	};

	struct window window_options = {
		WINDOW_DEFAULTS,
		.bridge = &editor->bridge,
	};

	if ((status = window_create(&editor->window, window_options)) != 0) {
		/* note: we expect the error site to print the error message, so we don't do it in between */
		/* note: only the endpoints print error messages, rest all log them */
		return status;
	}

	/* todo: add primitves etc to the editor, or atleast some resource arenas */
	/* todo: the shaders etc live in the model struct, nested deep to the level of primitives. so we never create them manually. */
	/* todo: have some kind of resource specifier file which links a model to it's respective shaders, ideally a header file in the code, makes things much easier, something like `resource.h` */
	/* todo: continue here, we were moving old main to this new editor/engine layout */

	/* todo: create a proper shader for gltf models with a lot of optional features (inside include guards) */
	struct shader *engine_shader = malloc(sizeof(struct shader));
	if (!engine_shader) {
		/* todo: cleanup the resources created above. an arena would be much helpful for that. */
		/* todo: or maybe we can just destroy the editor object which would check for allocated objects.. yes that sounds better here. */
		fprintf(stderr, "failed to allocate struct shader\n");
		return EXIT_FAILURE;
	}

	struct shader_options engine_shader_opts = {
		DEFAULT_SHADER_OPTIONS,
	};

	if ((status = shader_init_with_options(engine_shader, engine_shader_opts, shader_category_model)) != 0) {
		fprintf(stderr, "failed to initialize shader with opts\n");
		return EXIT_FAILURE;
	}

	/* todo: change this to use harfbuzz engine shaders */
	struct string *engine_vertex_shader_main;
	struct string *engine_fragment_shader_main;

	if ((engine_vertex_shader_main = string_create_from_file(ASSETS_DIR "shaders/gltf/shader.vert")) == NULL) {
		return EXIT_FAILURE;
	}

	if ((engine_fragment_shader_main = string_create_from_file(ASSETS_DIR "shaders/gltf/shader.frag")) == NULL) {
		return EXIT_FAILURE;
	}

	const char *vertex_sources[] = {
		shader_version,
		/* todo: preprocessor declarations */
		engine_vertex_shader_main->data,

	};

	const char *fragment_sources[] = {
		shader_version,
		/* todo: preprocessor declarations */
		engine_fragment_shader_main->data,
	};

	if ((status = shader_load_from_sources(engine_shader, vertex_sources, array_size(vertex_sources), fragment_sources, array_size(fragment_sources))) != 0) {
		fprintf(stderr, "failed to load shader\n");
		/* todo: free up resources */
		return EXIT_FAILURE;
	}

	string_destroy(engine_vertex_shader_main);
	string_destroy(engine_fragment_shader_main);

	return status;
}

i8 editor_run(struct editor_state *editor) {
	i8 status = 0;
	while (!window_close(editor->window)) {
		f64 current_frame = glfwGetTime();
		editor->delta_time = current_frame - editor->last_frame;
		editor->last_frame = current_frame;

		window_poll_events(editor->window);
		window_process_input(editor->window);
		window_clear_color(editor->window);
		process_input(editor, editor->window, editor->delta_time);

		window_swap_buffers(editor->window);
	}

	return status;
}

i8 editor_shutdown(struct editor_state *editor) {
	i8 status = 0;

	/* todo: do this till we have a shader_manager or something similar which can take care of the lifetimes responsibly */
	// shader_destroy((*editor->cengine->primitives)->shader);
	window_destroy(editor->window);
	/* todo: no need to destroy the editor state itself, it's allocated on the stack in main */

	return status;
}

void process_input(struct editor_state *editor, struct window *window, f64 delta_time) {
	// input_move_point_light(window, delta_time);
	fprintf(stderr, "process_input\n");
}

void mouse_move(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	// struct editor_state *editor = userdata;
	fprintf(stderr, "mouse_move\n");
}

void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	(void) window;
	// struct editor_state *editor = userdata;
	fprintf(stderr, "mouse_scroll\n");
}

void window_resize(void *userdata, struct window *window, i32 w, i32 h) {
	(void) userdata;
	(void) window;
	(void) w;
	(void) h;
	fprintf(stderr, "window resized!!!\n");
}
