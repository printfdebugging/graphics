#include <stdio.h>

#include "editor/filesystem/file.h"
#include "editor/layout/layout.h"

status editor_open(struct editor_state *editor, const char *filepath) {
	FILE *file = NULL;
	if (!(file = fopen(filepath, "r"))) {
		fprintf(stderr, "failed to open file: %s", filepath);
		return status_failure;
	}

	char *line = NULL;
	u64 linecap = 0;

	u32 ret = 0;
	while ((ret = getline(&line, &linecap, file) != -1)) {
		while (linecap > 0 && (line[linecap - 1] == '\n' || line[linecap - 1] == '\r'))
			linecap--;
		editor_row_append(editor, line, (u32) linecap);
	}

	free(line);
	fclose(file);
	return status_success;
}
