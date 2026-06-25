#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "cglm/struct.h"

#include "engine/core/defines.h"
#include "engine/engine.h"

#define WINDOW_DEFAULTS                                 \
	.width = 1400.0f,                               \
	.height = 800.0f,                               \
	.title = "floating",                            \
	.icon = ENGINE_ASSETS_DIR "icons/logo.png",     \
	.cursor = ENGINE_ASSETS_DIR "icons/cursor.png", \
	.cursor_size = 32,                              \
	.window = NULL,                                 \
	.color = { { 0.0f, 0.0f, 0.0f, 1.0 } },         \
	.bridge = NULL

struct window {
	i32 width;
	i32 height;
	const char *title;
	const char *icon;
	const char *cursor;
	i32 cursor_size;
	GLFWwindow *window;
	vec4s color;

	/** The bridge connects application code with the engine code. It's a bunch of function
	 * pointers which the window calls to pass on the events to the client code. The lifetime
	 * of bridge is managed by the client code, no engine subsystem calls free on this. */
	struct bridge *bridge;
};

status window_init(struct window *window, struct window window_options);
status window_set_icon(struct window *window, const char *path);
status window_set_cursor_icon(struct window *window, const char *path, i32 size);
status window_close(struct window *window);

/* todo: get window size */

void window_set_clear_color(struct window *window, vec4s color);
void window_process_input(struct window *window);
void window_poll_events(struct window *window);
void window_clear_color(struct window *window);
void window_swap_buffers(struct window *window);
void window_destroy(struct window *window);

#endif
