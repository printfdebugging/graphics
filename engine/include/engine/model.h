#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#include "cglm/struct.h"

#include "engine/primitive.h"
#include "engine/core/defines.h"

struct transform {
	mat4s model;
	mat4s view;
	mat4s projection;
};

struct mesh {
	struct primitive *primitives;
	u64 primitive_count;
};

struct node {
	b8 has_mesh;
	b8 has_translation;
	b8 has_rotation;
	b8 has_scale;
	b8 has_children;
	b8 has_matrix;

	vec3s translation;
	versors rotation;
	vec3s scale;
	mat4s matrix;

	const char *name;
	struct mesh *mesh;
	struct node **children;
	u64 child_count;
};

i8 node_create(struct node **node);
i8 node_destroy(struct node *node);
i8 mesh_create(struct mesh **mesh);
i8 mesh_destroy(struct mesh *mesh);

struct model {
	/** 'basepath' points to the model's directory. Paths to textures are stored relative
	 * to the model's location like 'emissive.jpg', so we need the directory path to
	 * get the absolute path for these assets. 'basepath' has a slash '/' at the end.
	 */
	char *basepath;
	struct transform transform;
	struct shader *shader;

	struct node *nodes;
	u64 node_count;

	// struct primitive **_primitives;
	// struct mesh **_meshes;
	// struct node **_nodes;
	//
	// u64 _node_count;
	// u64 _mesh_count;
	// u64 _primitive_count;
};

i8 model_create(struct model **model);
i8 model_destroy(struct model *model);

/** Loads model located in the `filepath` ?directory?. At the moment
 * only 'gltf' models are supported, the api is file format agnostic.
 * Returns a non-zero integer on failure to load the model.
 */
i8 model_load_from_file(struct model *model, const char *filepath);

#endif
