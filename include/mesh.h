#ifndef MESH_H
#define MESH_H

#include "cglm/struct.h"
#include "glad/glad.h"

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
        vec3s uv;
};

struct mesh {
        unsigned int vao;
        unsigned int ebo;
        unsigned int vbo_vertex;
        unsigned int vbo_color;
        unsigned int vbo_uv;
        unsigned int vbo_normal;

        unsigned int index_count;
        unsigned int vertex_count;
        unsigned int color_count;
        unsigned int uv_count;
        unsigned int normal_count;

        unsigned int index_stride;
        unsigned int vertex_stride;
        unsigned int color_stride;
        unsigned int uv_stride;
        unsigned int normal_stride;

        GLenum index_type;
        GLenum draw_mode;

        struct texture **textures;
        unsigned int texture_count;
};

struct mesh *mesh_create();
void mesh_destroy(struct mesh *mesh);
void mesh_load_vertices(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_indices(struct mesh *mesh, void *data, unsigned int count, GLenum type, unsigned int stride);
void mesh_load_colors(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_uv(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);
void mesh_load_normals(struct mesh *mesh, void *data, unsigned int count, unsigned int stride);

#endif
