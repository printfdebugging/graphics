#ifndef MODEL_H
#define MODEL_H

#include "cglm/struct.h"
#include <stdint.h>

#include "mesh.h"

struct Model {
        struct Mesh **mesh;
        uint32_t      meshCount;

        mat4s model;
        mat4s view;
        mat4s projection;
};

struct Model *modelCreate();

/* for now we are just loading gltf models. if in future
 * we load other models as well, we might want to either
 * pass a model type in this function, or create multiple functions
 * to load different types of models.*/
int  modelLoadFromFile(struct Model *model, const char *filepath);
void modelDestroy(struct Model *model);

#endif
