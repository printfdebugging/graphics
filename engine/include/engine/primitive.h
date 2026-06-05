#ifndef MESH_H
#define MESH_H

#include "cglm/struct.h"
#include "glad/glad.h"

#include "engine/texture.h"
#include "engine/shader.h"
#include "engine/core/defines.h"

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

        struct shader *shader;

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

// void primitive_load_shader(struct primitive *primitive, struct shader_options options);
// this will allow us to use one shader for multiple primitives.

// void primitive_use_shader(struct primitive *primitive);
// shader switching happens through this function, so that if it's a shader
// with similar options we can skip the glUseProgram call

/* the shader will be ref-counted, so the shader_destroy function has to
 * be changed to factor that in. the shaders will be stored in a global
 * in the shader.c file (or maybe somewhere else?). */

/* todo: do these once we are close to pbr, as then we will have an idea of what
 * we need in the shader_options struct and the one big shader, we would need to enable
 * and disable code out of that shader using some preprocessor macros which we append to the
 * shader using string operations. */
#endif
