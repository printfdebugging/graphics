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
	i32 linelen = 0;
	while ((linelen = (i32) getline(&line, &linecap, file)) != -1) {
		while (linelen > 1 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		editor_row_append(editor, line, (u32) linelen);
	}

	free(line);
	fclose(file);
	return status_success;
}
