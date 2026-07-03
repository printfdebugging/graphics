#include <string.h>

#include "engine/core/defines.h"

#include "editor/font/renderer.h"
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

void editor_row_append(struct editor_state *editor, char *line, u32 linelen) {
	editor->rows = realloc(editor->rows, sizeof(struct editor_row) * ((u32) editor->rows_count + 1));
	u32 at = editor->rows_count;
	struct editor_row *row = &editor->rows[at];

	row->raw_data_size = linelen;
	row->raw_data = malloc((u32) linelen + 1);
	memcpy(row->raw_data, line, (u32) linelen);
	row->raw_data[linelen] = '\0';

	font_renderer_init(&row->renderer_data);
	editor_row_layout(editor, row);

	editor->rows_count++;
}

void editor_row_layout(struct editor_state *editor, struct editor_row *row) {
	/* todo: handle errors here, or make these void returning functions with proper error messages. */
	font_renderer_load_text(&row->renderer_data, editor->font, row->raw_data);
	font_renderer_upload_to_gpu(&row->renderer_data);
	font_renderer_setup_quad_locations(&row->renderer_data, editor->font_shader);
}

/* bottom left of the window is 0,0 and top right is width,height */
// todo: seems like there is some multiplier at work, i should investigate this
f32 editor_row_get_screen_location(struct editor_state *editor, u32 row_number) {
	/* later: if (row_number > editor->screen_rows) */
	return (f32) ((u32) editor->window->height - ((u32) editor->font_size * row_number));
}
