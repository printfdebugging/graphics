#ifndef MODEL_H
#define MODEL_H

#include <cglm/struct.h>
#include <stdint.h>

#include "mesh.h"

struct model {
    struct mesh **mesh;
    uint32_t mesh_count;

    mat4s model;
    mat4s view;
    mat4s projection;
};

struct model *model_create();

/* for now we are just loading gltf models. if in future
 * we load other models as well, we might want to either
 * pass a model type in this function, or create multiple functions
 * to load different types of models.*/
int model_load_from_file(struct model *model, const char *filepath);

void model_destroy(struct model *model);

#endif
