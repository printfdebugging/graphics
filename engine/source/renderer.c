#include "cglm/struct.h"
#include "glad/glad.h"

#include "engine/renderer.h"
#include "engine/primitive.h"
#include "engine/node.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/core/defines.h"

void render_model(struct model *model) {
	for (u64 root_node_index = 0; root_node_index < model->root_nodes_count; ++root_node_index) {
		struct node *root_node = model->root_nodes[root_node_index];

		struct transform transform = model->transform;
		// transform.model = glms_mat4_mul(transform.model, node_get_mat4s_transform(root_node));
		render_node(root_node, transform);
	}
}

void render_node(struct node *node, struct transform transform) {
	/* todo: check if this is required here or not */
	transform.model = glms_mat4_mul(transform.model, node_get_mat4s_transform(node));
	if (node->mesh) {
		/* todo: figure out why this is never hit */
		render_mesh(node->mesh, transform);
	}

	for (u64 child_index = 0; child_index < node->children_count; ++child_index) {
		struct node *child_node = node->children[child_index];
		render_node(child_node, transform);
	}
}

void render_mesh(struct mesh *mesh, struct transform transform) {
	for (u64 primitive_index = 0; primitive_index < mesh->primitives_count; ++primitive_index) {
		struct primitive *primitive = &mesh->primitives[primitive_index];
		render_primitive(primitive, transform);
	}
}

void render_primitive(struct primitive *primitive, struct transform transform) {
	struct shader *shader = primitive->shader;

	shader_use(shader);
	shader_set_uniform(shader, model, Matrix4fv, 1, GL_FALSE, &transform.model.col[0].raw[0]);
	shader_set_uniform(shader, view, Matrix4fv, 1, GL_FALSE, &transform.view.col[0].raw[0]);
	shader_set_uniform(shader, projection, Matrix4fv, 1, GL_FALSE, &transform.projection.col[0].raw[0]);

	glBindVertexArray(primitive->vao);
	if (primitive->index_count) {
		glDrawElements(primitive->draw_mode, (i32) primitive->index_count, primitive->index_type, NULL);
	} else {
		glDrawArrays(primitive->draw_mode, 0, (i32) primitive->vertex_count);
	}
}
