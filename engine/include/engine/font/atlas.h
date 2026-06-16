#ifndef ENGINE_FONT_ATLAS_H
#define ENGINE_FONT_ATLAS_H

#include "engine/core/defines.h"
#include "engine/font/glyph.h"

#define TEXEL_SIZE	  8
#define ATLAS_PAGE_SIZE	  (TEXEL_SIZE * MB)
#define MAX_TEXTURE_COUNT 16

struct atlas_page {
	u32 texture;
	u32 texture_unit;
	u32 texture_buffer_object;

	/* in bytes */
	u32 capacity;
	u32 cursor;

	b32 initialized;
};

struct atlas {
	struct atlas_page pages[MAX_TEXTURE_COUNT];
	u32 used_pages_count;
};

void atlas_init(struct atlas *atlas);
i8 atlas_create_page(struct atlas *atlas);

/** Takes in the glyph `data`, uploads it to one of the atlas `pages` and
 * then sets the `atlas_page` and `atlas_offset` values in `info`. It automatically
 * creates a new page internally if the current page doesn't have enough space
 * for the glyph `data`.
 */
i8 atlas_upload_glyph(struct atlas *atlas, const char *data, u32 lenbytes, struct glyph_info *info);

/** todo: atlas_add_glyph_primitives() */
void atlas_destroy(struct atlas *atlas);

#endif
