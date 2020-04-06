#pragma once
#include <dvigl/core.h>

class Texture;

class Material
{
public:
    Material(Texture* albedo_tex, Texture* normal_tex);
    void bind();
    void release();

protected:
    Texture* albedo;
    Texture* normal;
};