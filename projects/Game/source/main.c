#include <stdlib.h>

#include "game.h"

int main(int argc, char *argv[]) {
	struct game_state game = { 0 };
	i8 status = 0;

	if ((status = game_initialize(&game, argc, argv)) != 0) {
		fprintf(stderr, "failed to initialize game\n");
		return EXIT_FAILURE;
	}

	/* todo: add meaning to these status messages i.e. create an enum and map them to things like a memory error, or file not found error.. etc. */
	if ((status = game_run(&game)) != 0) {
		fprintf(stderr, "failed to run the game\n");
		return EXIT_FAILURE;
	}

	/* this might return errors like failed to write the log/events to a file etc. errors */
	if ((status = game_shutdown(&game)) != 0) {
		fprintf(stderr, "failed to shutdown the game properly\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
