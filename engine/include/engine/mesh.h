#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include "engine/primitive.h"
#include "engine/core/defines.h"

struct mesh {
	struct primitive *primitives;
	u64 primitives_count;
};

void mesh_init(struct mesh *mesh);
void mesh_destroy(struct mesh *mesh);

#endif
