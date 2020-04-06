#include <dvigl/material.h>
#include <dvigl/texture.h>

Material::Material(Texture* albedo_tex, Texture* normal_tex)
{
    albedo = albedo_tex;
    normal = normal_tex;
}

void Material::bind()
{
    albedo->bind(GL_TEXTURE0);
    normal->bind(GL_TEXTURE1);
}

void Material::release() {}
