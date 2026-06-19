#include <stdlib.h>

#include "game.h"
#include "engine/core/defines.h"

int main(int argc, char *argv[]) {
	struct game_state game = { 0 };
	status rc = status_success;

	if (!(rc = game_initialize(&game, argc, argv)) ||
	    !(rc = game_run(&game)) ||
	    !(rc = game_shutdown(&game))) {
		fprintf(stderr, "failed to initialize game\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
