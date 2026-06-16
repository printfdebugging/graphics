#include <stdlib.h>

#include "editor.h"

int main(int argc, char *argv[]) {
	struct editor_state editor = { 0 };
	i8 status = 0;

	if ((status = editor_initialize(&editor, argc, argv)) != 0) {
		fprintf(stderr, "failed to initialize editor\n");
		return EXIT_FAILURE;
	}

	/* todo: add meaning to these status messages i.e. create an enum and map them to things like a memory error, or file not found error.. etc. */
	if ((status = editor_run(&editor)) != 0) {
		fprintf(stderr, "failed to run the editor\n");
		return EXIT_FAILURE;
	}

	/* this might return errors like failed to write the log/events to a file etc. errors */
	if ((status = editor_shutdown(&editor)) != 0) {
		fprintf(stderr, "failed to shutdown the editor properly\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
