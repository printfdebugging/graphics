#include "engine/model/mesh.h"

void mesh_init(struct mesh *mesh) {
	mesh->primitives_count = 0;
	mesh->primitives = NULL;
}

void mesh_destroy(struct mesh *mesh) {
	for (u64 primitive_index = 0; primitive_index < mesh->primitives_count; ++primitive_index) {
		primitive_destroy(&mesh->primitives[primitive_index]);
	}

	/* we don't free mesh here since it was allocated manually in
	 * model, so model would free it. */
	// free(mesh);
}
