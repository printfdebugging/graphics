#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "editor/editor.h"

/* this should later be changed to buffer specific operation as
 * a buffer can have smaller/larger font size than another buffer.*/
status editor_count_rows(struct editor_state *editor);

#endif
