#ifndef MESH_H
#define MESH_H

#include "cglm/struct.h"
#include "glad/glad.h"

#include "texture.h"
#include "core/defines.h"

enum primitive_attribute {
        PRIMITIVE_ATTRIBUTE_POSITION = 0,
        PRIMITIVE_ATTRIBUTE_COLOR = 1,
        PRIMITIVE_ATTRIBUTE_UV = 2,
        PRIMITIVE_ATTRIBUTE_NORMAL = 3,
        PRIMITIVE_ATTRIBUTE_COUNT,
};

/* do i also need to store the mesh vertex data? */
/* we need to compile shaders per mesh? or do we create meshes such that a shader which provides a few options can handle that. */
struct primitive {
        u32 vao;
        u32 ebo;
        u32 vbo_vertex;
        u32 vbo_color;
        u32 vbo_uv;
        u32 vbo_normal;

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

        /* todo: mesh has materials a shader */
        struct texture **textures;
        u64 texture_count;
};

struct primitive *primitive_create();
void primitive_destroy(struct primitive *primitive);
void primitive_load_vertices(struct primitive *primitive, void *data, u32 count, i32 stride);
void primitive_load_indices(struct primitive *primitive, void *data, u32 count, GLenum type, i32 stride);
void primitive_load_colors(struct primitive *primitive, void *data, u32 count, i32 stride);
void primitive_load_uv(struct primitive *primitive, void *data, u32 count, i32 stride);
void primitive_load_normals(struct primitive *primitive, void *data, u32 count, i32 stride);

#endif
