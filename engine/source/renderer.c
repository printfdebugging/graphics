#include "glad/glad.h"

#include "engine/renderer.h"
#include "engine/primitive.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/core/defines.h"

void render_model(struct model *model) {
	for (u64 i = 0; i < model->primitive_count; ++i) {
		render_primitive(model->primitives[i], model->transform);
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
