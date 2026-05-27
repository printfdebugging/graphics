#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "cglm/struct.h"

#include "core/defines.h"

struct window {
        i32 width;
        i32 height;
        const char *title;
        const char *icon;
        GLFWwindow *window;
        vec4s color;
};

struct window *window_create(i32 width, i32 height, const char *title, vec4s color);
void window_set_clear_color(struct window *window, vec4s color);
void window_process_input(struct window *window);
void window_poll_events(struct window *window);
void window_clear_color(struct window *window);
void window_swap_buffers(struct window *window);
void window_destroy(struct window *window);
bool window_close(struct window *window);
i8 window_set_icon(struct window *window, const char *path);
void window_scale_to_monitor_dpi(GLFWwindow *window);

#endif
