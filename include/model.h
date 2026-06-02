#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#include "cglm/struct.h"

#include "primitive.h"
#include "core/defines.h"

struct model {
        struct primitive **primitives;
        u64 primitive_count;

        mat4s model;
        mat4s view;
        mat4s projection;

        /** 'basepath' points to the model's directory. Paths to textures are stored relative
         * to the model's location like 'emissive.jpg', so we need the directory path to
         * get the absolute path for these assets. 'basepath' has a slash '/' at the end.
         */
        char *basepath;

        /* model has materials */
};

struct model *model_create();

/** Loads model located in the `filepath` ?directory?. At the moment
 * only 'gltf' models are supported, the api is file format agnostic.
 * Returns a non-zero integer on failure to load the model.
 */
i8 model_load_from_file(struct model *model, const char *filepath);

/* todo: create helpers to load various primitives, textures (and apparently other components related to animation etc) */

void model_destroy(struct model *model);

#endif
