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

	/** These options are used to map points to font size. We get these
	 * from the windowing subsystem and we set them in the following:
	 *
	 * 1. `hb_font_set_ptem`
	 * 2. `hb_font_set_scale`
	 *
	 * These affect the output values of the glyph we get from harfbuzz
	 * after shaping. Ideally we should set the scale values to 1, so that
	 * we upload an unscaled glyph to the atlas and then we can scale that
	 * glyph to the font size we want. `dpi` affects how the `fontsize`
	 * value be interpreted.
	 */
	i32 dpi;
	i32 xscale;
	i32 yscale;

	struct atlas *atlas;

	/** The size of this array will be `U16_MAX` since glyph indices are
	 * `u16`. The glyph at index `0` is reserved for the missing character
	 * glyph. Glyph index `0xFFFF` is reserved for indicating the position
	 * of a glyph deleted from the glyph stream. */
	struct glyph_info *glyph_cache;
	u32 cached_glyph_count;
	u32 cached_glyph_bytes;

	/* todo: we need some kind of indentifier for the font here. */
	/* todo: we would also need some kind of font renderer which holds and manages these font objects. */
};

status font_init_from_file(struct font *font, const char *filepath, i32 dpi);
status font_lookup_glyph(struct font *font, u16 glyph_index, struct glyph_info *glyph_info);
status font_destroy(struct font *font);

#endif
