#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/struct.h"

/* todo: a material has textures */
struct material {
	vec3s ambient;
	vec3s diffuse;
	vec3s specular;
	float shininess;
};

#endif
