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

#endif
