#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "hb-gpu.h"

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

struct shader *__font_renderer_create_default_shader();

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

	if ((editor->font_shader = __font_renderer_create_default_shader()) == NULL) {
		fprintf(stderr, "failed to create font shader\n");
		return 1;
	}

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
	shader_destroy(editor->font_shader);
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

struct shader *__font_renderer_create_default_shader() {
	const char *vert = ENGINE_ASSETS_DIR "shaders/harfbuzz/shader.vert";
	const char *frag = ENGINE_ASSETS_DIR "shaders/harfbuzz/shader.frag";

	struct string *vert_source = string_create_from_file(vert);
	struct string *frag_source = string_create_from_file(frag);
	if (!vert_source || !frag_source) {
		return NULL;
	}

	const char *vertex_main = vert_source->data;
	const char *fragment_main = frag_source->data;

	const char *version = "#version 330 core\n";
	const char *preamble = "#define HB_GPU_DEMO_DRAW\n";
	i8 status = 0;

	const char *vert_sources[] = {
		version,
		preamble,
		hb_gpu_shader_source(HB_GPU_SHADER_STAGE_VERTEX, HB_GPU_SHADER_LANG_GLSL),
		hb_gpu_draw_shader_source(HB_GPU_SHADER_STAGE_VERTEX, HB_GPU_SHADER_LANG_GLSL),
		vertex_main,
	};

	const char *frag_sources[] = {
		version,
		preamble,
		hb_gpu_shader_source(HB_GPU_SHADER_STAGE_FRAGMENT, HB_GPU_SHADER_LANG_GLSL),
		hb_gpu_draw_shader_source(HB_GPU_SHADER_STAGE_FRAGMENT, HB_GPU_SHADER_LANG_GLSL),
		fragment_main,
	};

	struct shader *shader = malloc(sizeof(struct shader));
	if (!shader) {
		fprintf(stderr, "failed to allocate shader\n");
		return NULL;
	}

	struct shader_options font_shader_opts = {
		DEFAULT_SHADER_OPTIONS,
	};

	if ((status = shader_init_with_options(shader, font_shader_opts, shader_category_font)) != 0) {
		free(shader);
		free(vert_source);
		free(frag_source);
		return NULL;
	}

	if ((status = shader_load_from_sources(shader, vert_sources, array_size(vert_sources), frag_sources, array_size(frag_sources))) != 0) {
		free(shader);
		free(vert_source);
		free(frag_source);
		return NULL;
	}

	free(vert_source);
	free(frag_source);
	return shader;
}
