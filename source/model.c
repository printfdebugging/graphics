#include "model.h"
#include "mesh.h"

#include <cgltf/cgltf.h>
#include <iso646.h>
#include <stdlib.h>
#include <string.h>

/* clang-format off */
static GLenum gltf_type_to_opengl_type(cgltf_component_type type) {
    switch (type) {
        case cgltf_component_type_r_8:      return GL_BYTE;
        case cgltf_component_type_r_8u:     return GL_UNSIGNED_BYTE;
        case cgltf_component_type_r_16:     return GL_SHORT;
        case cgltf_component_type_r_16u:    return GL_UNSIGNED_SHORT;
        case cgltf_component_type_r_32u:    return GL_UNSIGNED_INT;
        case cgltf_component_type_r_32f:    return GL_FLOAT;
        case cgltf_component_type_invalid:
        case cgltf_component_type_max_enum:
            fprintf(stderr, "error: invalid gltf component type\n");
            exit(1);
            break;
    }
}

static GLenum gltf_primitive_type_to_opengl_type(cgltf_primitive_type type) {
    switch (type) {
        case cgltf_primitive_type_points:         return GL_POINTS;
        case cgltf_primitive_type_lines:          return GL_LINES;
        case cgltf_primitive_type_line_loop:      return GL_LINE_LOOP;
        case cgltf_primitive_type_line_strip:     return GL_LINE_STRIP;
        case cgltf_primitive_type_triangles:      return GL_TRIANGLES;
        case cgltf_primitive_type_triangle_strip: return GL_TRIANGLE_STRIP;
        case cgltf_primitive_type_triangle_fan:   return GL_TRIANGLE_FAN;
        case cgltf_primitive_type_invalid:
        case cgltf_primitive_type_max_enum:
            fprintf(stderr, "error: invalid gltf primitive type\n");
            exit(1);
            break;
    }
}
/* clang-format on */

static void accessor_point_to_data(const cgltf_accessor *accessor, void **data, int *count, int *stride) {
    const cgltf_buffer_view *buffer_view = accessor->buffer_view;
    const int offset = accessor->offset + buffer_view->offset;
    const int data_size = accessor->count * cgltf_calc_size(accessor->type, accessor->component_type);

    /* todo: i heard that offset was in bytes, but data is a void * which is 64 bits = 8 bytes */
    *data = (buffer_view->buffer->data + offset);
    *count = accessor->count;
    *stride = buffer_view->stride;
}

struct model *model_create() {
    struct model *model = malloc(sizeof(struct model));
    if (!model) {
        fprintf(stderr, "failed to allocate model\n");
        return NULL;
    }
    *model = (struct model) { 0 };
    return model;
}

void model_destroy(struct model *model) {
    for (int i = 0; i < model->mesh_count; ++i) {
        mesh_destroy(*(model->mesh + i));
    }

    free(model->mesh);
    free(model);
}

/* todo: move it to a separate function in mesh.. */
/* todo: try out multiple models from the assets and see what works. */
/* note: assuming only one primitive in the primitives array for now */

int model_load(struct model *model, const char *filepath) {
    cgltf_data *data = NULL;
    cgltf_result result;
    const cgltf_options options = { 0 };

    result = cgltf_parse_file(&options, filepath, &data);
    if (result != cgltf_result_success) {
        fprintf(stderr, "failed to parse gltf file: %s\n", filepath);
        return 1;
    }

    result = cgltf_load_buffers(&options, data, filepath);
    if (result != cgltf_result_success) {
        fprintf(stderr, "failed to load gltf buffers for file: %s\n", filepath);
        return 1;
    }

    model->mesh = malloc(sizeof(struct mesh *) * data->meshes_count);
    model->mesh_count = data->meshes_count;

    /* iterate over all the meshes - meshes have primitives */
    for (int i = 0; i < data->meshes_count; ++i) {
        /*         struct mesh  */
        // contoinue here
        const cgltf_mesh *gltf_mesh = &data->meshes[i];
        struct mesh **meshptr = model->mesh + i;
        /* todo: check meshptr, if mesh_create was successful or not. */
        *meshptr = mesh_create();
        struct mesh *mesh = *meshptr;
        for (int j = 0; j < gltf_mesh->primitives_count; ++j) {
            const cgltf_primitive *primitive = &gltf_mesh->primitives[j];

            if (primitive->indices) {
                void *data = NULL;
                int count = 0;
                int stride = 0;
                GLenum type = gltf_type_to_opengl_type(primitive->indices->component_type);
                accessor_point_to_data(primitive->indices, &data, &count, &stride);
                mesh_load_indices(mesh, data, count, type, stride);
            }

            /* materials array defines various materials and their various properties
             * based on the pbr model?*/
            if (primitive->material) {
                cgltf_material *material = primitive->material;
            }

            if (primitive->type) {
                mesh->draw_mode = gltf_primitive_type_to_opengl_type(primitive->type);
            }

            const int attribute_count = primitive->attributes_count;
            for (int k = 0; k < attribute_count; ++k) {
                const cgltf_attribute *attribute = &primitive->attributes[k];
                void *data = NULL;
                int count = 0;
                int stride = 0;
                switch (attribute->type) {
                    case cgltf_attribute_type_position: {
                        accessor_point_to_data(attribute->data, &data, &count, &stride);
                        mesh_load_vertices(mesh, data, count, stride);
                    } break;
                    case cgltf_attribute_type_normal: {
                    } break;
                    case cgltf_attribute_type_texcoord: {
                        /* todo: check if this attribute->index property actually
                         * is an index in the textures array i.e. pointing to the texture
                         * these texture coordinates are for. */
                        accessor_point_to_data(attribute->data, &data, &count, &stride);
                        mesh_load_uv(mesh, data, count, stride);
                        /* where to load the texture? surely a texture is a part of the "mesh", so it
                         * probably should be loaded here since we are loading texture coordinates. */
                    } break;
                    case cgltf_attribute_type_color: {
                    } break;
                    case cgltf_attribute_type_invalid:
                    case cgltf_attribute_type_tangent:
                    case cgltf_attribute_type_joints:
                    case cgltf_attribute_type_weights:
                    case cgltf_attribute_type_custom:
                    case cgltf_attribute_type_max_enum:
                        fprintf(stderr, "attribute type not handled: %i\n", attribute->type);
                        break;
                }
            }
        }
    }

    cgltf_free(data);
    return 0;
}
