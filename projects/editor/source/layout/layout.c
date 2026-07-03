#include "engine/core/defines.h"

#include "editor/layout/layout.h"

status editor_count_rows(struct editor_state *editor) {
	if (!editor->window) {
		fprintf(stderr, "window not initialized\n");
		return status_failure;
	}

	struct window *window = editor->window;
	if (window->width < (i32) editor->font_size || window->height < (i32) editor->font_size) {
		fprintf(stderr, "window size not large enough to render rows\n");
		return status_failure;
	}

	editor->screen_rows = (u32) ((f32) window->height / editor->font_size);
	return status_success;
}
