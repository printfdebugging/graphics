#include <stdio.h>
#include <stdlib.h>

#include "hb.h"

#include "engine/font/font.h"
#include "engine/font/atlas.h"

static status __font_upload_glyph(struct font *font, u16 glyph_index, struct glyph_info *info);

status font_init_from_file(struct font *font, const char *filepath) {
	hb_blob_t *hb_blob = hb_blob_create_from_file(filepath);
	if (!hb_blob) {
		fprintf(stderr, "failed to read font file from path: %s\n", filepath);
		return status_failure;
	}

	font->face = hb_face_create(hb_blob, 0);
	if (!font->face) {
		fprintf(stderr, "failed to create face from font blob\n");
		hb_blob_destroy(hb_blob);
		font_destroy(font);
		return status_failure;
	}

	font->font = hb_font_create(font->face);
	if (!font->font) {
		fprintf(stderr, "failed to create font from face\n");
		font_destroy(font);
		return status_failure;
	}

	font->draw = hb_gpu_draw_create_or_fail();
	if (!font->draw) {
		fprintf(stderr, "failed to create gpu shape encoder\n");
		font_destroy(font);
		return status_failure;
	}

	font->atlas = malloc(sizeof(struct atlas));
	if (!font->atlas) {
		fprintf(stderr, "failed to allocate struct atlas\n");
		font_destroy(font);
		return status_failure;
	}

	atlas_init(font->atlas);

	status rc = status_success;
	if (!(rc = atlas_create_page(font->atlas))) {
		font_destroy(font);
		return rc;
	}

	font->glyph_cache = calloc(U16_MAX, sizeof(struct glyph_info));
	if (!font->glyph_cache) {
		fprintf(stderr, "failed to allocate struct glyph_info array\n");
		font_destroy(font);
		return status_failure;
	}

	font->cached_glyph_bytes = 0;
	font->cached_glyph_count = 0;
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
		.upem = yscale,
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
