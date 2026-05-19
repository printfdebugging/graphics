#ifndef MESH_H
#define MESH_H

#include "glad/glad.h"
#include "cglm/struct.h"

#include "texture.h"

enum MeshAttribute {
        MESH_ATTRIBUTE_POSITION = 0,
        MESH_ATTRIBUTE_COLOR    = 1,
        MESH_ATTRIBUTE_UV       = 2,
        MESH_ATTRIBUTE_NORMAL   = 3,
        MESH_ATTRIBUTE_COUNT,
};

struct Vertex {
        vec3s position;
        vec3s normal;
        vec3s textureCoordinates;
};

struct Mesh {
        unsigned int vao;
        unsigned int ebo;
        unsigned int vboVertex;
        unsigned int vboColor;
        unsigned int vboUV;
        unsigned int vboNormal;

        unsigned int indexCount;
        unsigned int vertexCount;
        unsigned int colorCount;
        unsigned int UVCount;
        unsigned int normalCount;

        unsigned int indexStride;
        unsigned int vertexStride;
        unsigned int colorStride;
        unsigned int UVStride;
        unsigned int normalStride;

        GLenum indexType;
        GLenum drawMode;

        struct Texture **textures;
        unsigned int     textureCount;
};

struct Mesh *meshCreate();

void meshDestroy(struct Mesh *mesh);
void meshLoadVertices(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride);
void meshLoadIndices(struct Mesh *mesh, void *data, unsigned int count, GLenum type, unsigned int stride);
void meshLoadColors(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride);
void meshLoadUV(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride);
void meshLoadNormals(struct Mesh *mesh, void *data, unsigned int count, unsigned int stride);

#endif
