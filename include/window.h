#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "cglm/struct.h"

#include "core/defines.h"

#define WINDOW_DEFAULTS                                     \
        .width = 3100.0f,                                   \
        .height = 1400.0f,                                  \
        .title = "floating",                                \
        .icon = ASSETS_DIR "logo.png",                      \
        .cursor = ASSETS_DIR "textures/sandbox/cursor.png", \
        .cursor_size = 32,                                  \
        .window = NULL,                                     \
        .color = { { 0.0f, 0.0f, 0.0f, 1.0 } }

/* todo: store various function pointers (callbacks) here, create some deafult handlers
 * and allow user to override them. */
struct window {
        i32 width;
        i32 height;
        const char *title;
        const char *icon;
        const char *cursor;
        i32 cursor_size;
        GLFWwindow *window;
        vec4s color;
};

struct window *window_create(struct window window);
struct window *__window_create(struct window window);
void window_set_clear_color(struct window *window, vec4s color);
void window_process_input(struct window *window);
void window_poll_events(struct window *window);
void window_clear_color(struct window *window);
void window_swap_buffers(struct window *window);
void window_destroy(struct window *window);
bool window_close(struct window *window);
i8 window_set_icon(struct window *window, const char *path);
i8 window_set_cursor_icon(struct window *window, const char *path, i32 size);
void window_scale_to_monitor_dpi(GLFWwindow *window);

#endif
