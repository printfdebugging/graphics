#include <stdlib.h>

#include "editor.h"
#include "engine/core/defines.h"

int main(int argc, char *argv[]) {
	struct editor_state editor = { 0 };
	status rc = status_success;

	if (!(rc = editor_initialize(&editor, argc, argv))) {
		fprintf(stderr, "failed to initialize editor\n");
		return EXIT_FAILURE;
	}

	/* todo: add meaning to these status messages i.e. create an enum and map them to things like a memory error, or file not found error.. etc. */
	if (!(rc = editor_run(&editor))) {
		fprintf(stderr, "failed to run the editor\n");
		return EXIT_FAILURE;
	}

	/* this might return errors like failed to write the log/events to a file etc. errors */
	if (!(rc = editor_shutdown(&editor))) {
		fprintf(stderr, "failed to shutdown the editor properly\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
