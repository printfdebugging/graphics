#ifndef RENDERER_H
#define RENDERER_H

#include "model.h"

void render_model(struct model *model);
void render_primitive(struct primitive *primitive, struct transform transform);

#endif
