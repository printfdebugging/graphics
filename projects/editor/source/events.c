#include "editor/events.h"
#include "editor/layout/layout.h"

void mouse_move_callback(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	(void) y;
	(void) window;
	(void) userdata;
	// struct editor_state *editor = userdata;
	fprintf(stderr, "mouse_move\n");
}

void mouse_scroll_callback(void *userdata, struct window *window, f64 x, f64 y) {
	(void) x;
	(void) window;
	struct editor_state *editor = userdata;

	if (y > 0) {
		if (editor->row_offset > 0)
			editor->row_offset--;
		else
			editor->row_offset = 0;
	} else {
		if (editor->row_offset < editor->rows_count - editor->screen_rows)
			editor->row_offset++;
		else
			editor->row_offset = editor->rows_count - editor->screen_rows;
	}
}

void window_resize_callback(void *userdata, struct window *window, i32 w, i32 h) {
	window->width = w;
	window->height = h;
	struct editor_state *editor = userdata;
	editor_count_rows(editor);
}
