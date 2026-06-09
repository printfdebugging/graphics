#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgltf.h"

#include "engine/model.h"
#include "engine/primitive.h"
#include "engine/texture.h"
#include "engine/core/defines.h"

static i8 gltf_component_type_to_gl_type(GLenum *type, cgltf_component_type gltf_type);
static i8 gltf_primitive_type_to_gl_type(GLenum *type, cgltf_primitive_type gltf_type);

/* simplify this that now we have get_basepath */
static char *get_image_path_from_uri(const char *model_file_path, const char *image_uri);

static void accessor_point_to_data(const cgltf_accessor *accessor, void **data, u64 *count, u64 *stride);

/** Returns the 'basepath' of the model directory with a slash '/' at the end.
 * The lifetime of the returned string is managed by the callee of this function.
 */
static char *get_basepath(const char *filepath);

/* todo: get the mesh <-> primitive names right */
static i8 gltf_load_nodes(struct model *model, cgltf_data *data);
static i8 gltf_load_mesh(struct mesh **mesh, cgltf_mesh *gltf_mesh, const char *basepath);
static i8 gltf_load_primitive_attributes(struct primitive *mesh, const struct cgltf_primitive *primitive);
static i8 gltf_load_primitive_material(struct primitive *mesh, const cgltf_primitive *primitive, const char *basepath);
static i8 gltf_load_primitive_indices(struct primitive *mesh, const cgltf_primitive *primitive);

static i8 gltf_component_type_to_gl_type(GLenum *type, cgltf_component_type gltf_type) {
	switch (gltf_type) {
		case cgltf_component_type_r_8:
			*type = GL_BYTE;
			return 0;
		case cgltf_component_type_r_8u:
			*type = GL_UNSIGNED_BYTE;
			return 0;
		case cgltf_component_type_r_16:
			*type = GL_SHORT;
			return 0;
		case cgltf_component_type_r_16u:
			*type = GL_UNSIGNED_SHORT;
			return 0;
		case cgltf_component_type_r_32u:
			*type = GL_UNSIGNED_INT;
			return 0;
		case cgltf_component_type_r_32f:
			*type = GL_FLOAT;
			return 0;
		case cgltf_component_type_invalid:
		case cgltf_component_type_max_enum:
		default:
			fprintf(stderr, "error: invalid gltf component type\n");
			return 1;
	}
}

static i8 gltf_primitive_type_to_gl_type(GLenum *type, cgltf_primitive_type gltf_type) {
	switch (gltf_type) {
		case cgltf_primitive_type_points:
			*type = GL_POINTS;
			return 0;
		case cgltf_primitive_type_lines:
			*type = GL_LINES;
			return 0;
		case cgltf_primitive_type_line_loop:
			*type = GL_LINE_LOOP;
			return 0;
		case cgltf_primitive_type_line_strip:
			*type = GL_LINE_STRIP;
			return 0;
		case cgltf_primitive_type_triangles:
			*type = GL_TRIANGLES;
			return 0;
		case cgltf_primitive_type_triangle_strip:
			*type = GL_TRIANGLE_STRIP;
			return 0;
		case cgltf_primitive_type_triangle_fan:
			*type = GL_TRIANGLE_FAN;
			return 0;
		case cgltf_primitive_type_invalid:
		case cgltf_primitive_type_max_enum:
		default:
			fprintf(stderr, "error: invalid gltf primitive type\n");
			return 1;
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
	char *image_path = malloc(base_path_size + image_path_size + 1);

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

	/* todo: i heard that offset was in bytes, but data is a void * which is 64 bits = 8 bytes */
	*data = ((u64 *) buffer_view->buffer->data + offset);
	*count = accessor->count;
	*stride = buffer_view->stride;
}

static char *get_basepath(const char *filepath) {
	const char *s0 = strrchr(filepath, '/');
	const char *s1 = strrchr(filepath, '\\');
	const char *slash = s0 ? (s1 && s1 > s0 ? s1 : s0) : s1;
	u64 prefix_index = (u64) (slash - filepath);
	u64 basepathsize = prefix_index + 2;

	char *basepath = malloc(basepathsize);
	if (!basepath) {
		fprintf(stderr, "failed to allocate memory for string\n");
		return NULL;
	}

	strncpy(basepath, filepath, (u64) prefix_index + 1);
	basepath[basepathsize - 1] = '\0';
	return basepath;
}

static i8 gltf_load_nodes(struct model *model, cgltf_data *data) {
	u64 node_count = data->nodes_count;
	struct node *nodes = malloc(sizeof(struct node) * node_count);
	if (!nodes) {
		fprintf(stderr, "failed to allocate memory for nodes\n");
		return 1;
	}

	for (u64 i = 0; i < node_count; ++i) {
		struct node *node = &nodes[i];
		cgltf_node *gltf_node = &data->nodes[i];

		/* this is for later as this refers to nodes which might not be loaded by now */
		/*
		if ((node->has_children = (gltf_node->children_count != 0)) == true) {
			u64 child_count = gltf_node->children_count;
			struct node **children = malloc(sizeof(struct node *) * child_count);
			if (!children) {
				fprintf(stderr, "failed to allocate memory for node->children\n");
				return 1;
			}
		}
		*/

		// if ((node->has_translation = gltf_node->has_translation) == true) glm_vec3_copy(gltf_node->translation, node->translation.raw);
		// if ((node->has_rotation = gltf_node->has_rotation) == true) glm_vec4_copy(gltf_node->rotation, node->rotation.raw);
		// if ((node->has_scale = gltf_node->has_scale) == true) glm_vec3_copy(gltf_node->scale, node->scale.raw);
		// if ((node->has_matrix = gltf_node->has_matrix) == true) glm_mat4_copy(glms_mat4_make(gltf_node->matrix).raw, node->matrix.raw);

		struct mesh *mesh = NULL;
		if (gltf_node->mesh) {
			i8 status = 0;
			if ((status = gltf_load_mesh(&mesh, gltf_node->mesh, model->basepath)) != 0) {
				return status;
			}
		}
		/* hacky for now, clean it up later */
		if (mesh) {
			node->has_mesh = true;
			node->mesh = mesh;
		} else {
			node->has_mesh = false;
		}
	}

	model->nodes = nodes;
	model->node_count = node_count;
	return 0;
}

static i8 gltf_load_mesh(struct mesh **mesh_out, cgltf_mesh *gltf_mesh, const char *basepath) {
	struct mesh *mesh = malloc(sizeof(struct mesh));
	if (!mesh) {
		fprintf(stderr, "failed to allocate memory for mesh\n");
		return 1;
	}

	mesh->primitive_count = gltf_mesh->primitives_count;
	mesh->primitives = malloc(sizeof(struct primitive) * mesh->primitive_count);
	if (!mesh->primitives) {
		fprintf(stderr, "failed to allocate memory for mesh->primitives\n");
		return 1;
	}

	for (u64 i = 0; i < mesh->primitive_count; ++i) {
		const cgltf_primitive *gltf_primitive = &gltf_mesh->primitives[i];
		struct primitive *primitive = &mesh->primitives[i];

		i8 status;

		if (gltf_primitive->type) {
			if ((status = gltf_primitive_type_to_gl_type(&primitive->draw_mode, gltf_primitive->type))) {
				/* handle error */
				return status;
			}
		}

		if (gltf_primitive->indices) {
			if ((status = gltf_load_primitive_indices(primitive, gltf_primitive))) {
				/* handle error */
				return status;
			}
		}

		/* todo: to a separate material loader function */
		if (gltf_primitive->material) {
			if ((status = gltf_load_primitive_material(primitive, gltf_primitive, basepath))) {
				/* handle error */
				return status;
			}
		}

		if ((status = gltf_load_primitive_attributes(primitive, gltf_primitive))) {
			/* handle error */
			return status;
		}
	}

	*mesh_out = mesh;
	return 0;
}

/* separate the definition and the declaration */
static i8 gltf_load_primitive_attributes(struct primitive *mesh, const struct cgltf_primitive *primitive) {
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
				return 1;
		}
	}
	return 0;
}

static i8 gltf_load_primitive_material(struct primitive *mesh, const cgltf_primitive *primitive, const char *basepath) {
	cgltf_material *material = primitive->material;
	i8 status = 0;
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
			if ((status = texture_load_from_file(mesh->textures[mesh->texture_count - 1], image_path))) {
				fprintf(stderr, "failed to load texture from file: %s\n", image_path);
				/* handle error, free memory etc */
				return status;
			}
			free((void *) image_path);
		}
	}
	return status;
}

static i8 gltf_load_primitive_indices(struct primitive *mesh, const cgltf_primitive *primitive) {
	void *index_data = NULL;
	u64 count = 0, stride = 0;
	GLenum type;
	i8 status;

	if ((status = gltf_component_type_to_gl_type(&type, primitive->indices->component_type)) != 0) {
		/* handle error */
		return status;
	}

	accessor_point_to_data(primitive->indices, &index_data, &count, &stride);
	primitive_load_indices(mesh, index_data, (u32) count, type, (i32) stride);
	return 0;
}

i8 model_create(struct model **model) {
	*model = malloc(sizeof(struct model));
	if (!*model) {
		fprintf(stderr, "failed to allocate model\n");
		return 1;
	}
	**model = (struct model) { 0 };
	return 0;
}

i8 model_destroy(struct model *model) {
	for (u64 i = 0; i < model->node_count; ++i) {
		node_destroy(&model->nodes[i]);
	}
	free(model->nodes);
	free(model->basepath);
	free(model);
	return 0;
}

i8 model_load_from_file(struct model *model, const char *filepath) {
	cgltf_data *data = NULL;
	const cgltf_options options = { 0 };
	cgltf_result result;
	i8 status;

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

	model->basepath = get_basepath(filepath);

	if ((status = gltf_load_nodes(model, data)) != 0) {
		fprintf(stderr, "failed to load meshes\n");
		return status;
	}

	cgltf_free(data);
	return 0;
}

i8 node_create(struct node **node) {
	struct node *node_ = malloc(sizeof(struct node));
	if (!node_) {
		fprintf(stderr, "failed to allocate memory for node\n");
		return 1;
	}

	*node_ = (struct node) { 0 };
	*node = node_;
	return 0;
}

i8 node_destroy(struct node *node) {
	if (node->has_mesh)
		// mesh destroy here
		free(node->mesh);
	/* not handled now */
	// if (node->children)
	// 	free(node->children);

	// free((void *) node->name);
	// free(node);
	return 0;
}

i8 mesh_create(struct mesh **mesh) {
	struct mesh *mesh_ = malloc(sizeof(struct mesh));
	if (!mesh_) {
		fprintf(stderr, "failed to allocate memory for mesh\n");
		return 1;
	}

	*mesh_ = (struct mesh) { 0 };
	*mesh = mesh_;
	return 0;
}

i8 mesh_destroy(struct mesh *mesh) {
	for (u64 i = 0; i < mesh->primitive_count; ++i) {
		primitive_destroy(&mesh->primitives[i]);
	}

	free(mesh);
	return 0;
}
