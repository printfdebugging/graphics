#include "engine/model.h"

enum resource_type {
	resource_type_static_model,
	resource_type_light,
	resource_type_count,
};

union resource {
	enum resource_type type;
	struct {
		const char *name;
		const char *model_path;
		const char *vertex_shader_path;
		const char *fragment_shader_path;
	} static_model;
};

union resource resources[] = {
	{
		.type = resource_type_static_model,
		.static_model = {
			.name = "Cylinder Engine",
			.model_path = ASSETS_DIR "models/CylinderEngine/glTF/2CylinderEngine.gltf",
			.vertex_shader_path = ASSETS_DIR "shaders/model/shader.vert",
			.fragment_shader_path = ASSETS_DIR "shaders/model/shader.frag",
		},
	},
};
