#ifndef ENGINE_FONT_GLYPH_H
#define ENGINE_FONT_GLYPH_H

#include "engine/core/defines.h"

struct extents {
	f64 min_x;
	f64 min_y;
	f64 max_x;
	f64 max_y;
};

struct point {
	f64 x;
	f64 y;
};

/** This is unmodified glyph info as we got from the font. We transform this and
 * create a `glyph_vertex` with scaling / coordinate conversion. todo: lookup and
 * write here what exactly we need to do. */
struct glyph_info {
	struct extents extents;
	f64 advance;
	i32 upem;

	/** These are the glyph's primitive locations in the texture on the gpu.
	 * `atlas_upload_glyph` sets these after uploading the glyph. `atlas_page`
	 * is for the `texture_unit` which has the glyph primitives and `atlas_offset`
	 * is the offset in `bytes` in that texture. */
	u32 atlas_offset;
	u32 atlas_page;

	b32 empty;

	/** This is allocated in bulk and ideally with `calloc`. That way `cached`
	 * will be set to `false` or `0` by default and then we can check for that
	 * to see if the glyph is there or not.
	 */
	b32 cached;
};

#endif
