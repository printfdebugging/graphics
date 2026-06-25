#include <stdio.h>
#include <stdlib.h>

#include "engine/core/defines.h"
#include "hb.h"

#include "engine/font/font.h"
#include "engine/font/atlas.h"

static status __font_upload_glyph(struct font *font, u16 glyph_index, struct glyph_info *info);

status font_init_from_file(struct font *font, const char *filepath) {
	hb_blob_t *hb_blob = NULL;
	status rc = status_success;

	if (!(hb_blob = hb_blob_create_from_file(filepath)) ||
	    !(font->face = hb_face_create(hb_blob, 0)) ||
	    !(font->font = hb_font_create(font->face)) ||
	    !(font->draw = hb_gpu_draw_create_or_fail())) {
		fprintf(stderr, "failed to read/initialize font file from path: %s\n", filepath);
		rc = status_failure;
		goto cleanup;
	}

	/* https://harfbuzz.github.io/harfbuzz-hb-font.html#hb-font-set-scale explains some details on how to set the font size properly */
	int dpi = 192;
	/* here we should load the font with font size 1 and only in the `font_renderer_load_text`
	 * should we specify the font size in the scale. that way we would have a
	 * scaling free cache and the quads will be scaled as we render things... */
	hb_font_set_ptem(font->font, (float) dpi);
	hb_font_set_scale(font->font, dpi * 1, dpi * 1);

	if (!(font->atlas = calloc(1, sizeof(struct atlas))) ||
	    !(font->glyph_cache = calloc(U16_MAX, sizeof(struct glyph_info)))) {
		fprintf(stderr, "failed to allocate memory\n");
		rc = status_failure;
		goto cleanup;
	}

	atlas_init(font->atlas);
	if (!(rc = atlas_create_page(font->atlas)))
		goto cleanup;

	font->cached_glyph_bytes = 0;
	font->cached_glyph_count = 0;
	hb_blob_destroy(hb_blob);
	return rc;

cleanup:
	hb_blob_destroy(hb_blob);
	font_destroy(font);
	return rc;
}

status font_lookup_glyph(struct font *font, u16 glyph_index, struct glyph_info *glyph_info) {
	struct glyph_info *info = &font->glyph_cache[glyph_index];
	status rc = status_success;

	if (info->cached == true) {
		*glyph_info = *info;
		return rc;
	}

	/* note: this sets the `cached` flag to true on success. */
	if (!(rc = __font_upload_glyph(font, glyph_index, info))) {
		fprintf(stderr, "failed to upload glyph to cache\n");
		return rc;
	}

	*glyph_info = *info;
	return rc;
}

static status __font_upload_glyph(struct font *font, u16 glyph_index, struct glyph_info *info) {
	i32 xscale, yscale;
	hb_blob_t *blob;
	hb_glyph_extents_t extents = {};

	hb_font_get_scale(font->font, &xscale, &yscale);
	hb_gpu_draw_clear(font->draw);
	hb_gpu_draw_glyph(font->draw, font->font, glyph_index);
	blob = hb_gpu_draw_encode(font->draw, &extents);
	u32 length = blob ? hb_blob_get_length(blob) : 0;

	*info = (struct glyph_info) {
		.extents.min_x = extents.x_bearing,
		.extents.max_x = extents.x_bearing + extents.width,
		.extents.min_y = extents.y_bearing,
		.extents.max_y = extents.y_bearing + extents.height,
		.advance = hb_font_get_glyph_h_advance(font->font, glyph_index),
		.upem = yscale, /* warn: should this be set to some value such that scaling becomes easier? */
		.empty = (length == 0),
		.cached = true,
	};

	status rc = status_success;
	if (!info->empty) {
		const char *data = hb_blob_get_data(blob, NULL);
		rc = atlas_upload_glyph(font->atlas, data, length, info);
		if (rc != status_success) {
			hb_gpu_draw_recycle_blob(font->draw, blob);
			return rc;
		}
	}

	font->cached_glyph_count += 1;
	font->cached_glyph_bytes += length;
	hb_gpu_draw_recycle_blob(font->draw, blob);

	return rc;
}

status font_destroy(struct font *font) {
	if (font->face)
		hb_face_destroy(font->face);
	if (font->font)
		hb_font_destroy(font->font);
	if (font->draw)
		hb_gpu_draw_destroy(font->draw);
	if (font->atlas)
		atlas_destroy(font->atlas);
	if (font->glyph_cache)
		free(font->glyph_cache);

	return 0;
}
