#include "glad/glad.h"

#include "engine/renderer.h"
#include "engine/primitive.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/core/defines.h"

void render_model(struct model *model) {
	for (u64 i = 0; i < model->node_count; ++i) {
		struct node *node = &model->nodes[i];
		if (node->has_mesh) {
			struct mesh *mesh = node->mesh;
			for (u64 j = 0; j < mesh->primitive_count; ++j) {
				struct primitive *primitive = &mesh->primitives[j];

				// vec4s translate;
				// mat4s rotate;
				// vec3s scale;
				// glms_decompose(node->matrix, &translate, &rotate, &scale);
				// node->translation = glms_vec4_copy3(translate);
				// node->rotation = glms_mat4_quat(rotate);
				// node->scale = scale;

				// struct transform trans = model->transform;
				// trans.model = (mat4s) { GLM_MAT4_IDENTITY_INIT };
				// trans.model = glms_translate(trans.model, node->translation);
				// trans.model = glms_rotate(trans.model, glms_quat_angle(node->rotation), glms_quat_axis(node->rotation));
				// trans.model = glms_scale(trans.model, node->scale);
				primitive->shader = model->shader; /* hack: for now */
				render_primitive(primitive, model->transform);
			}
		}
	}
}

void render_primitive(struct primitive *primitive, struct transform transform) {
	struct shader *shader = primitive->shader;

	/* this can be avoided by checking if we are using the same shader, though the check has
	 * to be here because the shader lives in the primitive. */
	glUseProgram(shader->program);
	shader_set_uniform(shader, "model", Matrix4fv, 1, GL_FALSE, &transform.model.col[0].raw[0]);
	shader_set_uniform(shader, "view", Matrix4fv, 1, GL_FALSE, &transform.view.col[0].raw[0]);
	shader_set_uniform(shader, "projection", Matrix4fv, 1, GL_FALSE, &transform.projection.col[0].raw[0]);

	glBindVertexArray(primitive->vao);
	if (primitive->index_count) {
		glDrawElements(primitive->draw_mode, (i32) primitive->index_count, primitive->index_type, NULL);
	} else {
		glDrawArrays(primitive->draw_mode, 0, (i32) primitive->vertex_count);
	}
}
