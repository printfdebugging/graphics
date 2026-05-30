#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#include "cglm/struct.h"

#include "mesh.h"
#include "core/defines.h"

struct model {
        struct mesh **mesh;
        u32 mesh_count;

        mat4s model;
        mat4s view;
        mat4s projection;

        /** Texture file paths in model files are local to the model
         * object, i.e. in the same directory as the model itself. So
         * we need `path` to find the absolute path of the texture
         * files.
         */
        char *path;
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
