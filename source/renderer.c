#include "glad/glad.h"

#include "renderer.h"
#include "primitive.h"
#include "model.h"
#include "shader.h"
#include "core/defines.h"

void render_model(struct model *model, struct shader *shader) {
        glUseProgram(shader->program);
        /* add model */
        shader_set_uniform(shader, "view", Matrix4fv, 1, GL_FALSE, &model->transform.view.col[0].raw[0]);
        shader_set_uniform(shader, "projection", Matrix4fv, 1, GL_FALSE, &model->transform.projection.col[0].raw[0]);

        for (u32 i = 0; i < model->primitive_count; ++i) {
                const struct primitive *mesh = model->primitives[i];
                glBindVertexArray(mesh->vao);
                if (mesh->index_count) {
                        glDrawElements(mesh->draw_mode, (i32) mesh->index_count, mesh->index_type, NULL);
                } else {
                        glDrawArrays(mesh->draw_mode, 0, (i32) mesh->vertex_count);
                }
        }
}

void render_primitive(struct primitive *primitive, struct shader *shader) {
}
