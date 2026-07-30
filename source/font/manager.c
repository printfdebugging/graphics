#include <stdio.h>
#include <stdlib.h>

#include "fontconfig/fontconfig.h"

#include "engine/font/manager.h"

static struct font_manager *g_font_manager = NULL;

/**
 * @brief
 * 	Uses fontconfig to find a font which can provide glyphs for
 * 	the `rune`. Loads the font directly into the static font
 * 	manager instance `g_font_manager`.
 */
static void font_manager_load_font(rune rune);

b8 font_manager_init() {
	if (g_font_manager)
		return true;
	if ((g_font_manager = calloc(1, sizeof(struct font_manager))) != NULL)
		return true;
	return false;
}

void font_manager_deinit() {
	if (!g_font_manager)
		return;

	for (u32 fontidx = 0; fontidx < block_count; ++fontidx)
		if (g_font_manager->cached[fontidx])
			font_destroy(&g_font_manager->fonts[fontidx]);

	free(g_font_manager);
	g_font_manager = NULL;
}

struct font *font_manager_get_font(rune rune) {
	if (!g_font_manager)
		return NULL;

	u32 blockidx;
	for (blockidx = 0; blockidx < block_count; ++blockidx)
		if (blocks[blockidx].start <= rune && rune <= blocks[blockidx].end)
			break;

	if (blockidx == block_count)
		return NULL;

	if (!g_font_manager->cached[blockidx])
		font_manager_load_font(rune);
	if (!g_font_manager->cached[blockidx])
		return NULL;
	return &g_font_manager->fonts[blockidx];
	return NULL;
}

void font_manager_load_font(rune rune) {
	if (!g_font_manager)
		return;
	perror("todo");
	return;
	/* fontconfig */
}
