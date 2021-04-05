#include <dvigl/material.h>
#include <dvigl/texture.h>

Material::Material(
    Texture* albedo_tex, Texture* normal_tex, Texture* metallic_tex, Texture* roughness_tex, Texture* ao_tex)
{
    albedo = albedo_tex;
    normal = normal_tex;
    metallic = metallic_tex;
    roughness = roughness_tex;
    ao = ao_tex;
}

void Material::bind()
{
    ao->bind(GL_TEXTURE4);
    metallic->bind(GL_TEXTURE3);
    roughness->bind(GL_TEXTURE2);
    normal->bind(GL_TEXTURE1);
    albedo->bind(GL_TEXTURE0);
}

void Material::release() {}
