#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <cglm/struct.h>

#include "shader.h"
#include "texture.h"

enum mesh_attribute {
    MESH_ATTRIBUTE_POSITION = 0,
    MESH_ATTRIBUTE_COLOR = 1,
    MESH_ATTRIBUTE_UV = 2,
    MESH_ATTRIBUTE_NORMAL = 3,
    MESH_ATTRIBUTE_COUNT,
};

struct vertex {
    vec3s position;
    vec3s normal;
    vec3s texture_coordinates;
};

struct mesh {
    /* mesh data */
    struct vertex *vertices;
    unsigned int vertex_count;

    int *indices;
    unsigned int index_count;

    struct texture *textures;
    unsigned int texture_count;

    /* render data */
    unsigned int vao;
    unsigned int vbo_color; /* we can either have them separately or in a single vbo.. */
    unsigned int vbo_uv;
    unsigned int vbo_normals;
    unsigned int ebo;

    unsigned int vbo_vertex;
    unsigned int vertex_stride;

    GLenum ebo_type;
};

struct mesh *mesh_create();
void mesh_destroy(struct mesh *mesh);
void mesh_load_vertices(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_indices(struct mesh *mesh, void *data, unsigned int count, GLenum type);
void mesh_load_colors(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_uv(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_normals(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);

#endif
