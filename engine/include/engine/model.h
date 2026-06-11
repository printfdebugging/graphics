#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#include "cglm/struct.h"

#include "engine/mesh.h"
#include "engine/node.h"
#include "engine/primitive.h"
#include "engine/core/defines.h"

struct transform {
	mat4s model;
	mat4s view;
	mat4s projection;
};

/* todo: move these to a separate file with their own init & destroy functions */
/* todo: but with introduction of arena/pool allocators, that won't be necessary as
 * we would do all the allocations for a model in it's pool and drop the whole
 * thing all at once once we are done, so no nested destroy calls :) */

struct model {
	struct transform transform;

	/** 'basepath' points to the model's directory. Paths to textures are
	 * stored relative to the model's location like 'emissive.jpg', so we
	 * need the directory path to get the absolute path for these assets.
	 * 'basepath' has a slash '/' at the end.
	 */
	char *basepath;

	/** An asset in glTF contains scenes, usually one per asset. A scene defines
	 * an array of root nodes. I don't quite know much about it yet, but I assume
	 * we would call render on these and then the calls will cascade to all the
	 * other nodes through the parent-child relationship. */
	struct node **root_nodes;
	u64 root_nodes_count;

	struct node *nodes;
	u64 nodes_count;

	struct mesh *meshes;
	u64 meshes_count;
};

void model_init(struct model *model);
void model_destroy(struct model *model);

/** Loads model located in the `filepath` ?directory?. At the moment
 * only 'gltf' models are supported, the api is file format agnostic.
 * Returns a non-zero integer on failure to load the model.
 */
i8 model_load_from_file(struct model *model, const char *filepath, struct shader *shader);
/* todo: remove the shader parameter from here, it's only for testing purposes. */

#endif
