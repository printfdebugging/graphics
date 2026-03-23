#include "logger.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <stdlib.h>

struct mesh *mesh_create() {
    struct mesh *mesh = malloc(sizeof(struct mesh));
    if (!mesh) {
        fprintf(stderr, "failed to allocate memory for mesh\n");
        return NULL;
    }

    *mesh = (struct mesh) { 0 };
    glGenVertexArrays(1, &mesh->vao);
    return mesh;
}

void mesh_destroy(struct mesh *mesh) {
    if (mesh->vbo_vertex) glDeleteBuffers(1, &mesh->vbo_vertex);
    if (mesh->vbo_color) glDeleteBuffers(1, &mesh->vbo_color);
    if (mesh->vbo_uv) glDeleteBuffers(1, &mesh->vbo_uv);
    if (mesh->ebo) glDeleteBuffers(1, &mesh->ebo);
    glDeleteVertexArrays(1, &mesh->vao);
    free(mesh);
}

/*
 * NOTE: if the vertices are packed together, then the stride would be
 *       the size of an individual chunk (before the attribute start repeating).
 *       also note that the last argument to `glVertexAttribPointer` is 0,
 *       that means that we are going to pass the pointer to the first data entry,
 *       not the start of the chunk.
 */
void mesh_load_vertices(struct mesh *mesh, float *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertex);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION);

    mesh->vertex_count = count;
    mesh->vertex_stride = stride;
}

void mesh_load_indices(struct mesh *mesh, int *data, unsigned int count, GLenum type) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * count, data, GL_STATIC_DRAW);

    mesh->ebo_count = count;
    mesh->ebo_type = type;
}

void mesh_load_colors(struct mesh *mesh, float *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_color);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_COLOR);
}

void mesh_load_uv(struct mesh *mesh, float *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_UV, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_UV);
}

void mesh_load_normals(struct mesh *mesh, float *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_NORMAL);
}
