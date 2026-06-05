#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include "cglm/struct.h"

#include "engine/core/defines.h"
#include "engine/engine.h"

#define WINDOW_DEFAULTS                                 \
        .width = 3100.0f,                               \
        .height = 1400.0f,                              \
        .title = "floating",                            \
        .icon = ENGINE_ASSETS_DIR "icons/logo.png",     \
        .cursor = ENGINE_ASSETS_DIR "icons/cursor.png", \
        .cursor_size = 32,                              \
        .window = NULL,                                 \
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
        struct app_bridge *bridge;
};

i8 window_create(struct window **window, struct window window_options);
void window_set_clear_color(struct window *window, vec4s color);
void window_process_input(struct window *window);
void window_poll_events(struct window *window);
void window_clear_color(struct window *window);
void window_swap_buffers(struct window *window);
void window_destroy(struct window *window);
bool window_close(struct window *window);
i8 window_set_icon(struct window *window, const char *path);
i8 window_set_cursor_icon(struct window *window, const char *path, i32 size);
i8 window_set_userdata(struct window *window, void *userdata);
void window_scale_to_monitor_dpi(GLFWwindow *window);

/* ideally we would set game_state pointer to the window user pointer on the game side when
 * a window is created. we would register static functions in window.c for those events i.e. the
 * handlers on the engine layer. those handlers will get the user pointer, cast it as the
 * engine_state and from that call the function set at the specific pointer like window_size_change_callback,
 * keyboard_input_callback etc with the pointer to game_state itself and the event info.
 *
 * then in the game code the callback will have access to the game_state and the event data. we would
 * have some way of making sure that the game code knows about the object it came from, like if there
 * are multiple windows, it makes sense to also pass something which helps us identify the window,
 * or the input device ... maybe engine_state isn't the right thing to store in the user pointer
 *
 * engine_state p */

#endif
