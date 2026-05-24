#include "renderer.h"

void render_model(struct model *model, struct shader *shader) {
        glUseProgram(shader->program);
        /* add model */
        shader_set_uniform(shader, "view", Matrix4fv, 1, GL_FALSE, &model->view.col[0].raw[0]);
        shader_set_uniform(shader, "projection", Matrix4fv, 1, GL_FALSE, &model->projection.col[0].raw[0]);

        for (int i = 0; i < model->mesh_count; ++i) {
                const struct mesh *mesh = model->mesh[i];
                glBindVertexArray(mesh->vao);
                if (mesh->index_count) {
                        glDrawElements(mesh->draw_mode, mesh->index_count, mesh->index_type, NULL);
                } else {
                        glDrawArrays(mesh->draw_mode, 0, mesh->vertex_count);
                }
        }
}
