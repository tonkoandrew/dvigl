#pragma once
#include <dvigl/core.h>

class Material;

class Mesh
{
public:
    Mesh();
    Mesh(struct aiMesh* mesh);
    void draw();
    void release();
    Material* material;
    int mat_idx;
    float bounding_radius = 0.0f;

protected:
    unsigned int vao;
    int vertex_count;
    float* vertex_data;
    float* normal_data;
    float* tangent_data;
    float* bitangent_data;
    float* textureCoord_data;
    void genVAO();
};