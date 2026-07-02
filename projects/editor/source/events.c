#include "editor/events.h"
#include "editor/layout/layout.h"

void mouse_move_callback(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	// struct editor_state *editor = userdata;
	fprintf(stderr, "mouse_move\n");
}

void mouse_scroll_callback(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	(void) window;
	// struct editor_state *editor = userdata;
	fprintf(stderr, "mouse_scroll\n");
}

void window_resize_callback(void *userdata, struct window *window, i32 w, i32 h) {
	window->width = w;
	window->height = h;
	struct editor_state *editor = userdata;
	editor_count_rows(editor);
}
