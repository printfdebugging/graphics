#include "unicode/unicode.h"

#include "engine/core/defines.h"

#include "editor/font/renderer.h"
#include "editor/layout/layout.h"

#define nul		0x00
#define space		0x20
#define newline		0x0a
#define horizontal_tab	0x09
#define carriage_return 0x0d

#define tabstop 3

/**
 * @brief
 * 	Iterates over the runes in a row and counds the
 * 	number of times it encounters a `tab` rune which
 * 	is `0x09` (ht).
 */
static u32 __editor_row_count_tabs(struct editor_row *row) {
	u32 tabcount = 0;
	for (u32 runeidx = 0; runeidx < row->runelen; ++runeidx)
		if (row->runes[runeidx] == horizontal_tab)
			++tabcount;
	return tabcount;
}

/**
 * @brief
 * 	The runes are printed with a `|` separator at the start
 * 	in order to distinguish between various runes.
 */
static void __editor_row_print_runes(struct editor_row *row) {
	for (u32 i = 0; i < row->runelen; ++i) {
		u32 rune = row->runes[i];
		u8 bytelen = rune_bytelen(rune);

		byte utf8[bytelen + 1];
		utf8[bytelen] = '\0';

		if (bytelen) {
			utf8_encode(rune, bytelen, utf8);
			fprintf(stderr, "|%s", utf8);
		} else {
			fprintf(stderr, "|?");
		}
	}

	fprintf(stderr, "\n");
}

/**
 * @brief
 * 	Counts the number of tabs in the row, then allocates a buffer
 * 	large enough to hold the new space glyphs (for the expanded tab),
 * 	then copies over all the runes, substituting the tabs with
 * 	`tabstop` number of spaces.
 */
static void __editor_row_substitute_tabs(struct editor_row *row) {
	u32 tabcount = __editor_row_count_tabs(row);
	if (tabcount) {
		u32 new_runelen = row->runelen + ((tabstop - 1) * tabcount);
		rune *new_runes = calloc(new_runelen, sizeof(rune));
		if (!new_runes) {
			fprintf(stderr, "failed to allocate new_runes for runes\n");
			return;
		}

		for (u32 new_runeidx = 0, runeidx = 0; runeidx < row->runelen && new_runeidx < new_runelen; ++runeidx) {
			if (row->runes[runeidx] == horizontal_tab) {
				for (u32 tabno = 0; tabno < tabstop; ++tabno)
					new_runes[new_runeidx++] = space;
			} else {
				new_runes[new_runeidx++] = row->runes[runeidx];
			}
		}

		free(row->runes);
		row->runes = new_runes;
		row->runelen = new_runelen;
	}
}

/**
 * @brief Replaces the newline runes with space runes.
 */
static void __editor_row_substitute_newlines(struct editor_row *row) {
	for (u32 runeidx = 0; runeidx < row->runelen; ++runeidx)
		if (row->runes[runeidx] == newline)
			row->runes[runeidx] = space;
}

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
	row->runelen = rune_count((u8 *) line, linelen);
	if (row->runelen == 0) {
		editor->rows_count++;
		return;
	}

	row->runes = calloc(row->runelen, sizeof(rune));
	if (!row->runes) {
		fprintf(stderr, "failed to allocate buffer for runes\n");
		return;
	}

	status rc = status_success;
	if (!(rc = utf8_decode_stream((u8 *) line, linelen, row->runes, row->runelen))) {
		editor->rows_count++;
		return;
	}

	/**
	 * @todo
	 * 	This is fine for now, but later we would want to
	 * 	preserve the original source buffer while doing so
	 * 	to a separate buffer which would be passed to
	 * 	the renderer.
	 */
	__editor_row_substitute_tabs(row);
	__editor_row_substitute_newlines(row);

	font_renderer_init(&row->renderer_data);
	editor_row_layout(editor, row);

	editor->rows_count++;
}

void editor_row_layout(struct editor_state *editor, struct editor_row *row) {
	/* todo: handle errors here, or make these void returning functions with proper error messages. */
	font_renderer_load_text(&row->renderer_data, editor->font, row->runes, row->runelen);
	font_renderer_upload_to_gpu(&row->renderer_data);
	font_renderer_setup_quad_locations(&row->renderer_data, editor->font_shader);
}

/* bottom left of the window is 0,0 and top right is width,height */
// todo: seems like there is some multiplier at work, i should investigate this
f32 editor_row_get_screen_location(struct editor_state *editor, u32 row_number) {
	/* later: if (row_number > editor->screen_rows) */
	return (f32) ((u32) editor->window->height - ((u32) editor->font_size * row_number));
}
