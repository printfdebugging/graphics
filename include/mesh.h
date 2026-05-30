#ifndef MESH_H
#define MESH_H

#include "cglm/struct.h"
#include "glad/glad.h"

#include "texture.h"
#include "core/defines.h"

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

/* do i also need to store the mesh vertex data? */
/* we need to compile shaders per mesh? or do we create meshes such that a shader which provides a few options can handle that. */
struct mesh {
        unsigned int vao;
        unsigned int ebo;
        unsigned int vbo_vertex;
        unsigned int vbo_color;
        unsigned int vbo_uv;
        unsigned int vbo_normal;

        u32 index_count;
        u32 vertex_count;
        u32 color_count;
        u32 uv_count;
        u32 normal_count;

        i32 index_stride;
        i32 vertex_stride;
        i32 color_stride;
        i32 uv_stride;
        i32 normal_stride;

        GLenum index_type;
        GLenum draw_mode;

        struct texture **textures;
        unsigned int texture_count;
};

struct mesh *mesh_create();
void mesh_destroy(struct mesh *mesh);
void mesh_load_vertices(struct mesh *mesh, void *data, u32 count, i32 stride);
void mesh_load_indices(struct mesh *mesh, void *data, u32 count, GLenum type, i32 stride);
void mesh_load_colors(struct mesh *mesh, void *data, u32 count, i32 stride);
void mesh_load_uv(struct mesh *mesh, void *data, u32 count, i32 stride);
void mesh_load_normals(struct mesh *mesh, void *data, u32 count, i32 stride);

#endif
