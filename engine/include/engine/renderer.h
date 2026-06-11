#ifndef RENDERER_H
#define RENDERER_H

#include "engine/model.h"

void render_model(struct model *model);

/** The 'transform' parameter is the transform of the parent node. */
void render_node(struct node *node, struct transform transform);
void render_mesh(struct mesh *mesh, struct transform transform);

/* todo: look into wireframe rendering
 * - it would be nice to render just the lines to see the boundaries of the objects
 * - glPolygonMode(GL_FRONT_AND_BACK, renderer->wireframe ? GL_LINE : GL_FILL);
 */
void render_primitive(struct primitive *primitive, struct transform transform);

#endif
