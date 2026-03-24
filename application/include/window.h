#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "cglm/struct.h"

#include <stdbool.h>

struct window {
    unsigned int width;
    unsigned int height;
    const char *title;
    const char *icon;
    GLFWwindow *window;
    vec4s color;

    // TODO: add callbacks and ways to register callbacks
    // of the form void (*callback)(sturct Window *window, void* data);
};

struct window *window_create(unsigned int width, unsigned int height, const char *title, vec4s color);
void window_set_clear_color(struct window *window, vec4s color);
void window_process_input(struct window *window);
void window_poll_events(struct window *window);
void window_clear_color(struct window *window);
void window_swap_buffers(struct window *window);
void window_destroy(struct window *window);
bool window_close(struct window *window);
int window_set_icon(struct window *window, const char *path);

#endif
