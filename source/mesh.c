#include "mesh.h"

#include <stdlib.h>

/* clang-format off */
static int glTypeToSize(GLenum type) {
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:  return 1;
        case GL_SHORT:          return sizeof(short);
        case GL_UNSIGNED_SHORT: return sizeof(unsigned short);
        case GL_UNSIGNED_INT:   return sizeof(unsigned int);
        case GL_FLOAT:          return sizeof(float);
        default:
            fprintf(stderr, "error: invalid opengl type\n");
            exit(1);
    }
}
/* clang-format on */

struct Mesh *meshCreate() {
    struct Mesh *mesh = malloc(sizeof(struct Mesh));
    if (!mesh) {
        fprintf(stderr, "failed to allocate memory for mesh\n");
        return NULL;
    }

    *mesh = (struct Mesh) { 0 };
    glGenVertexArrays(1, &mesh->vao);
    return mesh;
}

void meshDestroy(struct Mesh *mesh) {
    if (mesh->vboVertex) glDeleteBuffers(1, &mesh->vboVertex);
    if (mesh->vboColor) glDeleteBuffers(1, &mesh->vboColor);
    if (mesh->vboUV) glDeleteBuffers(1, &mesh->vboUV);
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
void meshLoadVertices(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboVertex);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION);
    mesh->vertexCount = count;
    mesh->vertexStride = stride;
}

void meshLoadIndices(struct Mesh *mesh, void *data, unsigned int count, GLenum type, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, glTypeToSize(type) * count, data, GL_STATIC_DRAW);
    mesh->indexCount = count;
    mesh->indexStride = stride;
    mesh->indexType = type;
}

void meshLoadColors(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboColor);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_COLOR);
    mesh->colorCount = count;
    mesh->colorStride = stride;
}

void meshLoadUV(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vboUV);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboUV);
    glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_UV, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_UV);
    mesh->UVCount = count;
    mesh->UVStride = stride;
}

void meshLoadNormals(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride) {
    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vboNormal);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vboNormal);
    glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(MESH_ATTRIBUTE_NORMAL);
    mesh->normalCount = count;
    mesh->normalStride = stride;
}
