#ifndef RENDERER_H
#define RENDERER_H

#include "model.h"
#include "shader.h"

void render_model(struct model *model, struct shader *shader);
void render_mesh(struct primitive *mesh, struct shader *shader);

#endif
