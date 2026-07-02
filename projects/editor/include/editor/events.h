#ifndef EDITOR_EVENTS_H
#define EDITOR_EVENTS_H

#include "engine/window.h"

void mouse_move_callback(void *userdata, struct window *window, f64 x, f64 y);
void mouse_scroll_callback(void *userdata, struct window *window, f64 x, f64 y);
void window_resize_callback(void *userdata, struct window *window, i32 w, i32 h);

#endif
