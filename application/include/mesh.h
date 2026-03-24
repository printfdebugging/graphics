#ifndef MESH_H
#define MESH_H

#include "glad/glad.h"

#include "shader.h"
#include "texture.h"

enum mesh_attribute {
    MESH_ATTRIBUTE_POSITION = 0,
    MESH_ATTRIBUTE_COLOR = 1,
    MESH_ATTRIBUTE_UV = 2,
    MESH_ATTRIBUTE_NORMAL = 3,
    MESH_ATTRIBUTE_COUNT,
};

struct mesh {
    unsigned int vao;
    unsigned int ebo;

    unsigned int vbo_vertex;
    unsigned int vertex_count;
    unsigned int vertex_stride;

    unsigned int vbo_color;
    unsigned int vbo_uv;
    unsigned int vbo_normals;

    unsigned int ebo_count;
    GLenum ebo_type;
};

struct mesh *mesh_create();
void mesh_destroy(struct mesh *mesh);
void mesh_load_vertices(struct mesh *mesh, float *data, unsigned int count, unsigned int stride);
void mesh_load_indices(struct mesh *mesh, int *data, unsigned int count, GLenum type);
void mesh_load_colors(struct mesh *mesh, float *data, unsigned int count, unsigned int stride);
void mesh_load_uv(struct mesh *mesh, float *data, unsigned int count, unsigned int stride);
void mesh_load_normals(struct mesh *mesh, float *data, unsigned int count, unsigned int stride);

#endif
