#include <stdlib.h>

#include "editor.h"
#include "engine/core/defines.h"

int main(int argc, char *argv[]) {
	struct editor_state editor = { 0 };
	status rc = status_success;

	if (!(rc = editor_initialize(&editor, argc, argv)) ||
	    !(rc = editor_run(&editor)) ||
	    !(rc = editor_shutdown(&editor))) {
		fprintf(stderr, "editor returned with a status_failure in main\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
