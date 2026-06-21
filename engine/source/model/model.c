#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgltf.h"

#include "engine/model/model.h"
#include "engine/model/node.h"
#include "engine/model/primitive.h"
#include "engine/texture.h"
#include "engine/core/defines.h"

static status gltf_component_type_to_gl_type(GLenum *type, cgltf_component_type gltf_type);
static status gltf_primitive_type_to_gl_type(GLenum *type, cgltf_primitive_type gltf_type);

/* simplify this that now we have get_basepath */
static char *get_image_path_from_uri(const char *model_file_path, const char *image_uri);

static void accessor_point_to_data(const cgltf_accessor *accessor, void **data, u64 *count, u64 *stride);

/** Returns the 'basepath' of the model directory with a slash '/' at the end.
 * The lifetime of the returned string is managed by the callee of this function.
 */
static char *get_basepath(const char *filepath);

/* todo: get the mesh <-> primitive names right */
static status gltf_load_primitive_attributes(struct primitive *mesh, const struct cgltf_primitive *primitive);
static status gltf_load_primitive_material(struct primitive *mesh, const cgltf_primitive *primitive, const char *basepath);
static status gltf_load_primitive_indices(struct primitive *mesh, const cgltf_primitive *primitive);

static status gltf_component_type_to_gl_type(GLenum *type, cgltf_component_type gltf_type) {
	switch (gltf_type) {
		case cgltf_component_type_r_8:
			*type = GL_BYTE;
			return status_success;
		case cgltf_component_type_r_8u:
			*type = GL_UNSIGNED_BYTE;
			return status_success;
		case cgltf_component_type_r_16:
			*type = GL_SHORT;
			return status_success;
		case cgltf_component_type_r_16u:
			*type = GL_UNSIGNED_SHORT;
			return status_success;
		case cgltf_component_type_r_32u:
			*type = GL_UNSIGNED_INT;
			return status_success;
		case cgltf_component_type_r_32f:
			*type = GL_FLOAT;
			return status_success;
		case cgltf_component_type_invalid:
		case cgltf_component_type_max_enum:
		default:
			fprintf(stderr, "error: invalid gltf component type\n");
			return status_failure;
	}
}

static status gltf_primitive_type_to_gl_type(GLenum *type, cgltf_primitive_type gltf_type) {
	switch (gltf_type) {
		case cgltf_primitive_type_points:
			*type = GL_POINTS;
			return status_success;
		case cgltf_primitive_type_lines:
			*type = GL_LINES;
			return status_success;
		case cgltf_primitive_type_line_loop:
			*type = GL_LINE_LOOP;
			return status_success;
		case cgltf_primitive_type_line_strip:
			*type = GL_LINE_STRIP;
			return status_success;
		case cgltf_primitive_type_triangles:
			*type = GL_TRIANGLES;
			return status_success;
		case cgltf_primitive_type_triangle_strip:
			*type = GL_TRIANGLE_STRIP;
			return status_success;
		case cgltf_primitive_type_triangle_fan:
			*type = GL_TRIANGLE_FAN;
			return status_success;
		case cgltf_primitive_type_invalid:
		case cgltf_primitive_type_max_enum:
		default:
			fprintf(stderr, "error: invalid gltf primitive type\n");
			return status_failure;
	}
}

/* note: todo:
 * - use memcopy instead of snprintf
 * - check for invalid last_separator and unsuccessful malloc
 */
static char *get_image_path_from_uri(const char *model_file_path, const char *image_uri) {
	const char *last_separator = strrchr(model_file_path, '/');
	u64 base_path_size = sizeof(char) * (u64) (last_separator - model_file_path + 1);
	u64 image_path_size = sizeof(char) * strlen(image_uri);
	char *image_path = calloc(1, base_path_size + image_path_size + 1);

	snprintf(image_path, base_path_size, "%s", model_file_path);
	image_path[base_path_size - 1] = '/';
	snprintf(image_path + base_path_size, image_path_size + 1, "%s", image_uri);
	return image_path;
}

static void accessor_point_to_data(const cgltf_accessor *accessor, void **data, u64 *count, u64 *stride) {
	const cgltf_buffer_view *buffer_view = accessor->buffer_view;
	const u64 offset = accessor->offset + buffer_view->offset;
	const u64 data_size = accessor->count * cgltf_calc_size(accessor->type, accessor->component_type);
	(void) data_size;

	/* note: gltf's offset is 'byteOffset' meaning that the pointer should be
	 * of u8 type and then adding the offset to it, pointer arithmetic will do
	 * it's magic. */
	*data = ((u8 *) buffer_view->buffer->data + offset);
	*count = accessor->count;
	*stride = buffer_view->stride;
}

static char *get_basepath(const char *filepath) {
	const char *s0 = strrchr(filepath, '/');
	const char *s1 = strrchr(filepath, '\\');
	const char *slash = s0 ? (s1 && s1 > s0 ? s1 : s0) : s1;
	u64 prefix_index = (u64) (slash - filepath);
	u64 basepathsize = prefix_index + 2;

	char *basepath = calloc(1, basepathsize);
	if (!basepath) {
		fprintf(stderr, "failed to allocate memory for string\n");
		return NULL;
	}

	strncpy(basepath, filepath, (u64) prefix_index + 1);
	basepath[basepathsize - 1] = '\0';
	return basepath;
}

/* separate the definition and the declaration */
static status gltf_load_primitive_attributes(struct primitive *mesh, const struct cgltf_primitive *primitive) {
	const u64 attribute_count = primitive->attributes_count;
	for (u64 k = 0; k < attribute_count; ++k) {
		const cgltf_attribute *attribute = &primitive->attributes[k];
		void *attr_data = NULL;
		u64 count = 0;
		u64 stride = 0;
		switch (attribute->type) {
			case cgltf_attribute_type_position: {
				accessor_point_to_data(attribute->data, &attr_data, &count, &stride);
				primitive_load_vertices(mesh, attr_data, (u32) count, (i32) stride);
			} break;
			case cgltf_attribute_type_normal: {
			} break;
			case cgltf_attribute_type_texcoord: {
				/* todo: check if this attribute->index property actually
				 * is an index in the textures array i.e. pointing to the texture
				 * these texture coordinates are for. */
				accessor_point_to_data(attribute->data, &attr_data, &count, &stride);
				primitive_load_uv(mesh, attr_data, (u32) count, (i32) stride);
				/* where to load the texture? surely a texture is a part of the
				 * "mesh", so it probably should be loaded here since we are
				 * loading texture coordinates. */
			} break;
			case cgltf_attribute_type_color: {
			} break;
			case cgltf_attribute_type_invalid:
			case cgltf_attribute_type_tangent:
			case cgltf_attribute_type_joints:
			case cgltf_attribute_type_weights:
			case cgltf_attribute_type_custom:
			case cgltf_attribute_type_max_enum:
			default:
				fprintf(stderr, "attribute type not handled: %i\n", attribute->type);
				return status_failure;
		}
	}
	return status_success;
}

static status gltf_load_primitive_material(struct primitive *mesh, const cgltf_primitive *primitive, const char *basepath) {
	cgltf_material *material = primitive->material;
	status rc = status_success;
	if (material->pbr_metallic_roughness.base_color_texture.texture) {
		// material->pbr_metallic_roughness.base_color_texture

		/* note: cgltf images either have buffer_view or uri, they can't
		 * have both */
		/* todo: create a texture in the mesh, increment count. */
		cgltf_image *image = material->pbr_metallic_roughness
					 .base_color_texture.texture->image;
		if (image->buffer_view) {
			/* todo: handle this case */
			fprintf(stderr, "image is in the buffer_view\n");
			exit(1);
		} else if (image->uri) {
			char *image_path = get_image_path_from_uri(basepath, image->uri);
			mesh->texture_count += 1;
			mesh->textures = realloc(mesh->textures, sizeof(struct texture **) * mesh->texture_count);

			mesh->textures[mesh->texture_count - 1] = texture_create();
			/* note: textures should be stored globally, indentified by
			 * their filepath so that we don't create multiple textures
			 * for the same texture. */
			if (!(rc = texture_load_from_file(mesh->textures[mesh->texture_count - 1], image_path))) {
				fprintf(stderr, "failed to load texture from file: %s\n", image_path);
				/* handle error, free memory etc */
				return rc;
			}
			free((void *) image_path);
		}
	}
	return rc;
}

static status gltf_load_primitive_indices(struct primitive *mesh, const cgltf_primitive *primitive) {
	void *index_data = NULL;
	u64 count = 0, stride = 0;
	GLenum type;
	status rc = status_success;

	if (!(rc = gltf_component_type_to_gl_type(&type, primitive->indices->component_type))) {
		return rc;
	}

	accessor_point_to_data(primitive->indices, &index_data, &count, &stride);
	primitive_load_indices(mesh, index_data, (u32) count, type, (i32) stride);
	return rc;
}

status model_init(struct model *model) {
	model->basepath = NULL;
	model->nodes = NULL;
	model->meshes = NULL;
	model->nodes_count = 0;
	model->meshes_count = 0;

	model->transform = (struct transform) {
		.model = { GLM_MAT4_IDENTITY_INIT },
		/* todo: view and the project matrix defaults */
	};
	return status_success;
}

/* todo: destroy nodes and meshes */
void model_destroy(struct model *model) {
	for (u64 node_index = 0; node_index < model->nodes_count; ++node_index)
		node_destroy(&model->nodes[node_index]);
	for (u64 mesh_index = 0; mesh_index < model->meshes_count; ++mesh_index)
		mesh_destroy(&model->meshes[mesh_index]);

	free(model->basepath);
	free(model);
}

status model_load_from_file(struct model *model, const char *filepath, struct shader *shader) {
	char *basepath = NULL;
	cgltf_data *data = NULL;
	const cgltf_options options = { 0 };
	cgltf_result result = cgltf_result_success;
	status rc = status_success;

	if ((result = cgltf_parse_file(&options, filepath, &data)) != cgltf_result_success) {
		fprintf(stderr, "failed to parse gltf file: %s\n", filepath);
		return status_failure;
	}

	if ((result = cgltf_load_buffers(&options, data, filepath)) != cgltf_result_success) {
		fprintf(stderr, "failed to load gltf buffers for file: %s\n", filepath);
		rc = status_failure;
		goto cleanup;
	}

	if (!(model->basepath = get_basepath(filepath))) {
		rc = status_failure;
		goto cleanup;
	}

	u64 nodes_count = data->nodes_count;
	model->nodes_count = nodes_count;

	if (!(model->nodes = calloc(nodes_count, sizeof(struct node)))) {
		fprintf(stderr, "failed to allocate for model\n");
		rc = status_failure;
		goto cleanup;
	}

	struct node *nodes = model->nodes;
	for (u64 node_index = 0; node_index < nodes_count; ++node_index) {
		cgltf_node *gltf_node = &data->nodes[node_index];
		struct node *node = &nodes[node_index];
		node_init(node);

		if (gltf_node->name) {
			node->name = strdup(gltf_node->name);
		}

		/* allocate an array for the child node pointers */
		if (gltf_node->children_count != 0) {
			u64 children_count = gltf_node->children_count;
			node->children_count = children_count;

			if (!(node->children = calloc(children_count, sizeof(struct node *)))) {
				fprintf(stderr, "failed to allocate memory for node->children\n");
				rc = status_failure;
				goto cleanup;
			}
		}

		/* if none of these are specified, 'node_init' sets the default values. */
		if (gltf_node->has_matrix) {
			vec4s _translate;
			mat4s _rotate;
			vec3s _scale;
			glms_decompose(glms_mat4_make(gltf_node->matrix), &_translate, &_rotate, &_scale);
			node->translation = glms_vec4_copy3(_translate);
			node->rotation = glms_mat4_quat(_rotate);
			node->scale = _scale;
		} else {
			if (gltf_node->has_translation)
				glm_vec3_copy(gltf_node->translation, node->translation.raw);
			if (gltf_node->has_rotation)
				glm_vec4_copy(gltf_node->rotation, node->rotation.raw);
			if (gltf_node->has_scale)
				glm_vec3_copy(gltf_node->scale, node->scale.raw);
		}
	}

	u64 mesh_count = data->meshes_count;
	model->meshes_count = mesh_count;
	if (!(model->meshes = calloc(mesh_count, sizeof(struct mesh)))) {
		fprintf(stderr, "failed to allocate meshes\n");
		rc = status_failure;
		goto cleanup;
	}

	struct mesh *meshes = model->meshes;
	for (u64 mesh_index = 0; mesh_index < mesh_count; ++mesh_index) {
		cgltf_mesh *gltf_mesh = &data->meshes[mesh_index];
		struct mesh *mesh = &meshes[mesh_index];
		mesh_init(mesh);

		u64 primitive_count = gltf_mesh->primitives_count;
		mesh->primitives_count = primitive_count;
		if (!(mesh->primitives = calloc(primitive_count, sizeof(struct primitive)))) {
			fprintf(stderr, "failed to allocate mesh->primitives\n");
			rc = status_failure;
			goto cleanup;
		}

		for (u64 primitive_index = 0; primitive_index < primitive_count; ++primitive_index) {
			cgltf_primitive *gltf_primitive = &gltf_mesh->primitives[primitive_index];
			struct primitive *primitive = &mesh->primitives[primitive_index];
			primitive_create_vertex_array(primitive);
			/* todo: do this properly with some shader manager */
			primitive->shader = shader;

			if (gltf_primitive->type) {
				if (!(rc = gltf_primitive_type_to_gl_type(&primitive->draw_mode, gltf_primitive->type))) {
					goto cleanup;
				}
			}

			if (gltf_primitive->indices) {
				if (!(rc = gltf_load_primitive_indices(primitive, gltf_primitive))) {
					goto cleanup;
				}
			}

			/* todo: to a separate material loader function */
			if (gltf_primitive->material) {
				if (!(rc = gltf_load_primitive_material(primitive, gltf_primitive, basepath))) {
					goto cleanup;
				}
			}

			if (!(rc = gltf_load_primitive_attributes(primitive, gltf_primitive))) {
				goto cleanup;
			}
		}
	}

	/* create the parent-child node hierarchy */
	for (u64 node_index = 0; node_index < nodes_count; ++node_index) {
		cgltf_node *gltf_node = &data->nodes[node_index];
		struct node *node = &model->nodes[node_index];

		if (gltf_node->children_count != 0) {
			for (u64 child_index = 0; child_index < gltf_node->children_count; ++child_index) {
				i64 index_in_nodes_array = gltf_node->children[child_index] - data->nodes;
				node->children[child_index] = &model->nodes[index_in_nodes_array];
				node->children[child_index]->parent = node;
			}
		}
	}

	/* load the root nodes from the scene*/
	if (data->scene && data->scene->nodes_count != 0) {
		u64 root_nodes_count = data->scene->nodes_count;
		model->root_nodes_count = root_nodes_count;
		if (!(model->root_nodes = calloc(root_nodes_count, sizeof(struct node *)))) {
			rc = status_failure;
			goto cleanup;
		}

		struct node **root_nodes = model->root_nodes;
		/* setup root node pointers */
		for (u64 node_index = 0; node_index < root_nodes_count; ++node_index) {
			cgltf_node *gltf_node = data->scene->nodes[node_index];
			i64 index_in_nodes_array = gltf_node - data->nodes;
			root_nodes[node_index] = &model->nodes[index_in_nodes_array];
			root_nodes[node_index]->parent = NULL; /* set it explicitly, already done in node_init though. */
		}
	}

	/* setup meshes for all the nodes */
	for (u64 node_index = 0; node_index < nodes_count; ++node_index) {
		cgltf_node *gltf_node = &data->nodes[node_index];
		struct node *node = &nodes[node_index];
		if (gltf_node->mesh) {
			i64 mesh_index_in_array = gltf_node->mesh - data->meshes;
			node->mesh = &model->meshes[mesh_index_in_array];
		}
	}

	cgltf_free(data);
	return rc;

cleanup:
	free(model->basepath);
	cgltf_free(data);
	return rc;
}
