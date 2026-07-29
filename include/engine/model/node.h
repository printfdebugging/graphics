#ifndef ENGINE_NODE_H
#define ENGINE_NODE_H

/* todo: this pulls in a lot of other cglm headers we don't use,
 * so ideally we should be including the specific headers we use.
 * that would be a nice execuse to look into the function
 * implementations.
 */
#include "cglm/struct.h"

#include "engine/model/mesh.h"
#include "engine/core/defines.h"

struct node {
	/** We don't store the transform matrix, that way if there is one, we
	 * decompose that into it's respective TRS components. If there are just
	 * TRS components on node, we store those. if they are not there at all,
	 * we use some default values, see 'node_init'. */
	vec3s translation;
	versors rotation;
	vec3s scale;

	/** Name is allocated by 'strdup' and it's lifetime is managed by the
	 * node, it should be free'd in 'node_destroy'. */
	const char *name;

	struct mesh *mesh;

	struct node *parent;

	/** Just an array of pointers to other nodes. This array itself is dynamically
	 * allocated and should be free'd in 'node_destroy', but the nodes themselves
	 * are not managed by this node, the model will free them. */
	struct node **children;
	u64 children_count;
};

void node_init(struct node *node);
void node_destroy(struct node *node);

mat4s node_get_mat4s_transform(struct node *node);

/* todo: create a pair of helper functions which convert trs to matrix and vice versa */

#endif
