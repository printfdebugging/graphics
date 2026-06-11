#include "cglm/struct.h"

#include "engine/node.h"
#include "engine/mesh.h"

void node_init(struct node *node) {
	node->mesh = NULL;
	node->parent = NULL;
	node->children = NULL;
	node->name = NULL;
	node->children_count = 0;

	node->translation = (vec3s) { { 0, 0, 0 } };
	node->rotation = (versors) { .w = 1 };
	node->scale = (vec3s) { { 1, 1, 1 } };
}

void node_destroy(struct node *node) {
	free((void *) node->name);
	free(node->children);
	mesh_destroy(node->mesh);
}

mat4s node_get_mat4s_transform(struct node *node) {
	mat4s transform = { GLM_MAT4_IDENTITY_INIT };
	transform = glms_translate(transform, node->translation);
	transform = glms_rotate(transform, glms_quat_angle(node->rotation), glms_quat_axis(node->rotation));
	transform = glms_scale(transform, node->scale);
	return transform;
}
