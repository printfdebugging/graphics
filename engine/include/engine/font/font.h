#ifndef ENGINE_FONT_FONT_H
#define ENGINE_FONT_FONT_H

#include "hb.h"
#include "hb-gpu.h"

#include "engine/core/defines.h"
#include "engine/font/atlas.h"
#include "engine/font/glyph.h"

struct font {
	hb_face_t *face;
	hb_font_t *font;
	hb_gpu_draw_t *draw;

	struct atlas *atlas;

	/** The size of this array will be `U16_MAX` since glyph indices are
	 * `u16`. The glyph at index `0` is reserved for the missing character
	 * glyph. Glyph index `0xFFFF` is reserved for indicating the position
	 * of a glyph deleted from the glyph stream. */
	struct glyph_info *glyph_cache;
	u32 cached_glyph_count;
	u32 cached_glyph_bytes;
};

i8 font_init_from_file(struct font *font, const char *filepath);
i8 font_lookup_glyph(struct font *font, u16 glyph_index, struct glyph_info *glyph_info);
i8 font_destroy(struct font *font);

#endif
