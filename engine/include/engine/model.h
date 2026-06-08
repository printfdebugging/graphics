#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#include "cglm/struct.h"

#include "engine/primitive.h"
#include "engine/core/defines.h"

struct transform {
        mat4s model;
        mat4s view;
        mat4s projection;
};

struct model {
        struct primitive **primitives;
        u64 primitive_count;

        struct transform transform;

        /** 'basepath' points to the model's directory. Paths to textures are stored relative
         * to the model's location like 'emissive.jpg', so we need the directory path to
         * get the absolute path for these assets. 'basepath' has a slash '/' at the end.
         */
        char *basepath;

        /* model has materials */
};

i8 model_create(struct model **model);
i8 model_destroy(struct model *model);

/** Loads model located in the `filepath` ?directory?. At the moment
 * only 'gltf' models are supported, the api is file format agnostic.
 * Returns a non-zero integer on failure to load the model.
 */
i8 model_load_from_file(struct model *model, const char *filepath);

#endif
