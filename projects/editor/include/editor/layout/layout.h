#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "editor/editor.h"

/* note: this should later be changed to buffer specific operation as
 * a buffer can have smaller/larger font size than another buffer.
 *
 * since this would be changed to be a buffer specific thing, it
 * should happen after the buffer data has been loaded and all the
 * formatting calculations are taken into account. like a row can be
 * larger than other rows. emacs i think does this, one row with a
 * constant height, text quads can be smaller/larger in that.
 */
status editor_count_rows(struct editor_state *editor);

void editor_row_append(struct editor_state *editor, char *line, u32 linelen);

/* this is where we prepare the quads for the row. this involves all the glyph
 * quad structures... */
void editor_row_layout(struct editor_state *editor, struct editor_row *row);

f32 editor_row_get_screen_location(struct editor_state *editor, u32 row_number);

#endif
