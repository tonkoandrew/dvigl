#pragma once
#include <dvigl/core.h>

class Texture;

class Material
{
public:
    Material(Texture* albedo_tex, Texture* normal_tex, Texture* metallic_tex, Texture* roughness_tex, Texture* ao_tex);
    void bind();
    void release();

protected:
    Texture* albedo;
    Texture* normal;
    Texture* metallic;
    Texture* roughness;
    Texture* ao;
};