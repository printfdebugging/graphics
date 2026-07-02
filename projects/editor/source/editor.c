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

#include "editor/editor.h"
#include "editor/font/renderer.h"

void mouse_move(void *userdata, struct window *window, f64 x, f64 y);
void mouse_scroll(void *userdata, struct window *window, f64 x, f64 y);
void window_resize(void *userdata, struct window *window, i32 w, i32 h);

/* this is for keyboard inputs which we don't get via callbacks, but we check for
 * explicitly in the main loop maybe there's another way, let's ask nitrix later */
void process_input(struct editor_state *editor, struct window *window, f64 delta_time);

status font_renderer_init_default_shader(struct shader *shader);

status editor_initialize(struct editor_state *editor, int argc, char *argv[]) {
	status rc = status_success;
	(void) argc;
	(void) argv;

	editor->bridge = (struct bridge) {
		.mouse_move = mouse_move,
		.mouse_scroll = mouse_scroll,
		.window_resize = window_resize,
	};

	if (!(editor->window = calloc(1, sizeof(struct window))) ||
	    !(editor->font = calloc(1, sizeof(struct font))) ||
	    !(editor->font_shader = calloc(1, sizeof(struct shader))) ||
	    !(editor->font_renderer = calloc(1, sizeof(struct font_renderer)))) {
		fprintf(stderr, "failed to allocate memory for editor\n");
		rc = status_failure;
		goto cleanup;
	}

	struct window window_options = {
		WINDOW_DEFAULTS,
		.bridge = &editor->bridge,
	};

	i32 dpi;
	if (!(rc = window_init(editor->window, window_options)) ||
	    !(rc = window_get_monitor_dpi(editor->window, &dpi))) {
		goto cleanup;
	}

	editor->text = "abcdefghijklmnopqrstuvwxyz";
	editor->font_filepath = ENGINE_ASSETS_DIR "fonts/IosevkaNerdFont-Regular.ttf";
	editor->font_size = 30;

	if (!(rc = font_init_from_file(editor->font, editor->font_filepath, dpi)) ||
	    !(rc = font_renderer_init(editor->font_renderer)) ||
	    !(rc = font_renderer_init_default_shader(editor->font_shader)) ||
	    !(rc = font_renderer_load_text(editor->font_renderer, editor->font, editor->text))) {
		goto cleanup;
	}

	shader_use(editor->font_shader);
	font_renderer_upload_to_gpu(editor->font_renderer);
	font_renderer_setup_quad_locations(editor->font_renderer, editor->font_shader);

	return rc;

cleanup:
	editor_shutdown(editor);
	return rc;
}

status editor_run(struct editor_state *editor) {
	status rc = status_success;
	glfwShowWindow(editor->window->window);

	while (!window_close(editor->window)) {
		f64 current_frame = glfwGetTime();
		editor->delta_time = current_frame - editor->last_frame;
		editor->last_frame = current_frame;

		window_poll_events(editor->window);
		window_process_input(editor->window);
		window_clear_color(editor->window);
		process_input(editor, editor->window, editor->delta_time);

		i32 xscale, yscale;
		hb_font_get_scale(editor->font->font, &xscale, &yscale);
		struct font_renderer_options renderer_opts = {
			.font_size = editor->font_size / (f32) yscale,
			.transformation_matrix = glms_ortho(0, (f32) editor->window->width, 0, (f32) editor->window->height, 0.0f, 100.0f),
		};

		font_renderer_render_text(editor->font_renderer, editor->font, editor->font_shader, renderer_opts);
		window_swap_buffers(editor->window);
	}

	return rc;
}

status editor_shutdown(struct editor_state *editor) {
	status rc = status_success;

	/* todo: do this till we have a shader_manager or something similar which can take care of the lifetimes responsibly */
	// shader_destroy((*editor->cengine->primitives)->shader);
	shader_destroy(editor->font_shader);
	window_destroy(editor->window);
	/* todo: no need to destroy the editor state itself, it's allocated on the stack in main */

	return rc;
}

void process_input(struct editor_state *editor, struct window *window, f64 delta_time) {
	// input_move_point_light(window, delta_time);
	// fprintf(stderr, "process_input\n");
	(void) editor;
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
	window->width = w;
	window->height = h;
}

status font_renderer_init_default_shader(struct shader *shader) {
	const char *vert = ASSETS_DIR "shaders/harfbuzz/shader.vert";
	const char *frag = ASSETS_DIR "shaders/harfbuzz/shader.frag";

	struct string *vert_source = NULL;
	struct string *frag_source = NULL;
	status rc = status_success;

	if (!(vert_source = string_create_from_file(vert)) ||
	    !(frag_source = string_create_from_file(frag))) {
		rc = status_failure;
		goto cleanup;
	};

	const char *vertex_main = vert_source->data;
	const char *fragment_main = frag_source->data;
	const char *version = "#version 330 core\n";
	const char *preamble = "#define HB_GPU_DEMO_DRAW\n";

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

	struct shader_options font_shader_opts = {
		DEFAULT_SHADER_OPTIONS,
	};

	if (!(rc = shader_init_with_options(shader, font_shader_opts, shader_category_font)) ||
	    !(rc = shader_load_from_sources(shader, vert_sources, array_size(vert_sources), frag_sources, array_size(frag_sources)))) {
		goto cleanup;
	}

cleanup:
	string_destroy(vert_source);
	string_destroy(frag_source);
	return rc;
}
