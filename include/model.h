#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include <cglm/struct.h>
#include "mesh.h"

struct model {
    struct mesh **mesh;
    uint32_t mesh_count;

    mat4s view;
    mat4s projection;
};

struct model *model_create();
int model_load(struct model *model, const char *filepath);
void model_destroy(struct model *model);

#endif
