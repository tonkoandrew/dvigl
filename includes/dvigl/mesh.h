#pragma once
#include <dvigl/core.h>

class Texture;

class Mesh {
public:
    Mesh();
    Mesh(int w, int h);
    Mesh(struct aiMesh* mesh);
    void draw();
    void release();
    Texture* texture;
    int mat_idx;

protected:
    unsigned int vao;
    int vertex_count;
    float* vertex_data;
    float* normal_data;
    float* tangent_data;
    float* textureCoord_data;
    void genVAO();
};