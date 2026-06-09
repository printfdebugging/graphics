#include "engine/model.h"

struct resource {
	const char *name;
	const char *model_path;
	const char *vertex_shader_path;
	const char *fragment_shader_path;
};

struct resource resources[] = {
	{
	    .name = "Cylinder Engine",
	    .model_path = ASSETS_DIR "models/CylinderEngine/glTF/2CylinderEngine.gltf",
	    .vertex_shader_path = ASSETS_DIR "shaders/model/shader.vert",
	    .fragment_shader_path = ASSETS_DIR "shaders/model/shader.frag",
	},
};
