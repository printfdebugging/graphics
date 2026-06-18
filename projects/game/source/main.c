#include <stdlib.h>

#include "game.h"
#include "engine/core/defines.h"

int main(int argc, char *argv[]) {
	struct game_state game = { 0 };
	status rc = status_success;

	if (!(rc = game_initialize(&game, argc, argv))) {
		fprintf(stderr, "failed to initialize game\n");
		return EXIT_FAILURE;
	}

	/* todo: add meaning to these status messages i.e. create an enum and map them to things like a memory error, or file not found error.. etc. */
	if (!(rc = game_run(&game))) {
		fprintf(stderr, "failed to run the game\n");
		return EXIT_FAILURE;
	}

	/* this might return errors like failed to write the log/events to a file etc. errors */
	if (!(rc = game_shutdown(&game))) {
		fprintf(stderr, "failed to shutdown the game properly\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
