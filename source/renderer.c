#include "renderer.h"

void renderModel(struct Model *model, struct Shader *shader) {
    glUseProgram(shader->program);
    /* add model */
    shaderSetUniform(shader, "view", Matrix4fv, 1, GL_FALSE, &model->view.col[0].raw[0]);
    shaderSetUniform(shader, "projection", Matrix4fv, 1, GL_FALSE, &model->projection.col[0].raw[0]);

    for (int i = 0; i < model->meshCount; ++i) {
        const struct Mesh *mesh = model->mesh[i];
        glBindVertexArray(mesh->vao);
        if (mesh->indexCount) {
            glDrawElements(mesh->drawMode, mesh->indexCount, mesh->indexType, NULL);
        } else {
            glDrawArrays(mesh->drawMode, 0, mesh->vertexCount);
        }
    }
}
