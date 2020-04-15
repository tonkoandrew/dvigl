#defshader vertex
#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#defshader fragment
#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

// GBuffer
uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D materialInfoTexture;
uniform sampler2D worldPosTexture;
uniform sampler2D ssaoTexture;

uniform bool visualize_albedo;
uniform bool visualize_normals;
uniform bool visualize_metallic;
uniform bool visualize_roughness;
uniform bool visualize_ao;
uniform bool visualize_world_position;

// ============================= ++++++++++++++++++++++++++++++++=== ==================
struct DirLight
{
    vec3 direction;

    float intensity;
    vec3 lightColour;
};

struct PointLight
{
    vec3 position;

    float intensity;
    vec3 lightColour;
    float attenuationRadius;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float intensity;
    vec3 lightColour;
    float attenuationRadius;

    float cutOff;
    float outerCutOff;
};

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 20
#define MAX_SPOT_LIGHTS 5
const float PI = 3.14159265359;
// IBL
uniform int reflectionProbeMipCount;
uniform bool computeIBL;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Lighting
uniform sampler2D shadowmap;

uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;
// uniform mat4 viewInverse;
// uniform mat4 projectionInverse;
uniform mat4 lightSpaceViewProjectionMatrix;

ivec4 numDirPointSpotLights = ivec4(1, 20, 0, 0);

// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the
// strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float normDotHalf = dot(normal, halfway);
    float normDotHalf2 = normDotHalf * normDotHalf;

    float numerator = a2;
    float denominator = normDotHalf2 * (a2 - 1.0) + 1.0;
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

float GeometrySchlickGGX(float cosTheta, float roughness)
{
    float r = (roughness + 1.0);
    float k = (roughness * roughness) / 8.0;

    float numerator = cosTheta;
    float denominator = cosTheta * (1.0 - k) + k;

    return numerator / max(denominator, 0.001);
}

// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    return GeometrySchlickGGX(max(dot(normal, viewDir), 0.0), roughness)
        * GeometrySchlickGGX(max(dot(normal, lightDir), 0.0), roughness);
}

// Calculates the amount of specular light. Since diffuse(refraction) and specular(reflection) are mutually exclusive,
// we can also use this to determine the amount of diffuse light
// Taken from UE4's implementation which is faster and basically identical to the usual Fresnel calculations:
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity)
{
    return max(baseReflectivity + (1.0 - baseReflectivity) * pow(2, (-5.55473 * cosTheta - 6.98316) * cosTheta), 0.0);
}

float CalculateShadow(vec3 fragPos, vec3 normal, vec3 fragToLight)
{
    vec4 fragPosLightClipSpace = lightSpaceViewProjectionMatrix * vec4(fragPos, 1.0);
    vec3 ndcCoords = fragPosLightClipSpace.xyz / fragPosLightClipSpace.w;
    vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

    float shadow = 0.0;
    float currentDepth = depthmapCoords.z;

    // Add shadow bias to avoid shadow acne. However too much bias can cause peter panning
    float shadowBias = 0.003;

    // Perform Percentage Closer Filtering (PCF) in order to produce soft shadows
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            float sampledDepthPCF = texture(shadowmap, depthmapCoords.xy + (texelSize * vec2(x, y))).r;
            shadow += currentDepth > sampledDepthPCF + shadowBias ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (currentDepth > 1.0)
        shadow = 0.0;
    shadow = 0.0;
    return shadow;
}

// TODO: Need to also add multiple shadow support
vec3 CalculateDirectionalLightRadiance(
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 baseReflectivity)
{
    vec3 directLightIrradiance = vec3(0.0);

    for (int i = 0; i < numDirPointSpotLights.x; ++i)
    {
        vec3 lightDir = normalize(-dirLights[i].direction);
        vec3 halfway = normalize(lightDir + fragToView);
        vec3 radiance = dirLights[i].intensity * dirLights[i].lightColour;

        // Cook-Torrance Specular BRDF calculations
        float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, fragToView, lightDir, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(lightDir, normal), 0.0)
            + 0.001; // Prevents any division by zero
        vec3 specular = numerator / denominator;

        // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
        vec3 diffuse = diffuseRatio * albedo / PI;

        // Add the light's radiance to the irradiance sum
        directLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, lightDir), 0.0)
            * (1.0 - CalculateShadow(fragPos, normal, lightDir));
    }

    return directLightIrradiance;
}

vec3 CalculatePointLightRadiance(
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 baseReflectivity)
{
    vec3 pointLightIrradiance = vec3(0.0);
    float distance_multiplier = 1.0;

    for (int i = 0; i < numDirPointSpotLights.y; ++i)
    {
        vec3 fragToLight = normalize(pointLights[i].position - fragPos);
        vec3 halfway = normalize(fragToView + fragToLight);
        float fragToLightDistance = length(pointLights[i].position - fragPos) * distance_multiplier;

        // Attenuation calculation (based on Epic's UE4 falloff model)
        float d = fragToLightDistance / pointLights[i].attenuationRadius;
        float d2 = d * d;
        float d4 = d2 * d2;
        float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
        float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
        vec3 radiance = pointLights[i].intensity * pointLights[i].lightColour * attenuation;

        // Cook-Torrance Specular BRDF calculations
        float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0)
            + 0.001; // Prevents any division by zero
        vec3 specular = numerator / denominator;

        // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
        vec3 diffuse = diffuseRatio * albedo / PI;

        // Add the light's radiance to the irradiance sum
        pointLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
    }

    return pointLightIrradiance;
}

vec3 CalculateSpotLightRadiance(
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 baseReflectivity)
{
    vec3 spotLightIrradiance = vec3(0.0);
    float distance_multiplier = 1.0;

    for (int i = 0; i < numDirPointSpotLights.z; ++i)
    {
        vec3 fragToLight = normalize(spotLights[i].position - fragPos);
        vec3 halfway = normalize(fragToView + fragToLight);
        float fragToLightDistance = length(spotLights[i].position - fragPos) * distance_multiplier;

        // Attenuation calculation (based on Epic's UE4 falloff model)
        float d = fragToLightDistance / spotLights[i].attenuationRadius;
        float d2 = d * d;
        float d4 = d2 * d2;
        float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

        // Check if it is in the spotlight's circle
        float theta = dot(normalize(spotLights[i].direction), -fragToLight);
        float difference = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / difference, 0.0, 1.0);
        float attenuation
            = intensity * (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
        vec3 radiance = spotLights[i].intensity * spotLights[i].lightColour * attenuation;

        // Cook-Torrance Specular BRDF calculations
        float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0)
            + 0.001; // Prevents any division by zero
        vec3 specular = numerator / denominator;

        // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
        vec3 diffuse = diffuseRatio * albedo / PI;

        // Add the light's radiance to the irradiance sum
        spotLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
    }

    return spotLightIrradiance;
}

// ============================= ++++++++++++++++++++++++++++++++=== ==================

void main()
{

    vec3 FragPos = texture(worldPosTexture, TexCoords).xyz;
    // Sample textures
    vec3 albedo = texture(albedoTexture, TexCoords).rgb;
    float albedoAlpha = texture(albedoTexture, TexCoords).w;
    vec3 normal = texture(normalTexture, TexCoords).rgb;
    float metallic = texture(materialInfoTexture, TexCoords).r;
    float unclampedRoughness = texture(materialInfoTexture, TexCoords).g; // Used for indirect specular (reflections)
    float roughness = max(unclampedRoughness,
        0.4); // Used for calculations since specular highlights will be too fine, and will cause flicker
    float materialAO = texture(materialInfoTexture, TexCoords).b;
    float sceneAO = texture(ssaoTexture, TexCoords).r;
    float ao = min(materialAO, sceneAO);

    vec3 fragPos = FragPos;

    vec3 fragToView = normalize(viewPos - fragPos);
    vec3 reflectionVec = reflect(-fragToView, normal);

    // Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse
    // (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
    vec3 baseReflectivity = vec3(0.04);
    baseReflectivity = mix(baseReflectivity, albedo, metallic);

    // Calculate per light radiance for all of the direct lighting
    vec3 directLightIrradiance = vec3(0.0);
    directLightIrradiance += CalculateDirectionalLightRadiance(
        albedo, normal, metallic, roughness, fragPos, fragToView, baseReflectivity);
    directLightIrradiance
        += CalculatePointLightRadiance(albedo, normal, metallic, roughness, fragPos, fragToView, baseReflectivity);
    directLightIrradiance
        += CalculateSpotLightRadiance(albedo, normal, metallic, roughness, fragPos, fragToView, baseReflectivity);

    directLightIrradiance = clamp(directLightIrradiance, 0.0, 0.8);

    // Calcualte ambient IBL for both diffuse and specular
    vec3 ambient = vec3(0.05) * albedo * ao;
    if (computeIBL)
    {
        vec3 specularRatio = FresnelSchlick(max(dot(normal, fragToView), 0.0), baseReflectivity);
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        vec3 indirectDiffuse = texture(irradianceMap, normal).rgb * albedo * diffuseRatio;

        vec3 prefilterColour
            = textureLod(prefilterMap, reflectionVec, unclampedRoughness * (reflectionProbeMipCount - 1)).rgb;
        vec2 brdfIntegration = texture(brdfLUT, vec2(max(dot(normal, fragToView), 0.0), roughness)).rg;
        vec3 indirectSpecular = prefilterColour * (specularRatio * brdfIntegration.x + brdfIntegration.y);

        ambient = (indirectDiffuse + indirectSpecular) * ao;
    }

    vec3 final_color;
    final_color.r = ambient.r + directLightIrradiance.r > 1 ? 1 : ambient.r + directLightIrradiance.r;
    final_color.g = ambient.g + directLightIrradiance.g > 1 ? 1 : ambient.g + directLightIrradiance.g;
    final_color.b = ambient.b + directLightIrradiance.b > 1 ? 1 : ambient.b + directLightIrradiance.b;

    FragColor = vec4(final_color, 1.0);
    // FragColor = vec4(ambient + directLightIrradiance, 1.0);
    // =============+++++++++++++++++============

    if (visualize_albedo)
    {
        FragColor = vec4(albedo, 1.0);
    }

    if (visualize_normals)
    {
        FragColor = vec4((normal * 0.5 + 0.5), 1.0);
    }

    if (visualize_metallic)
    {
        FragColor = vec4(vec3(metallic), 1.0);
    }

    if (visualize_roughness)
    {
        FragColor = vec4(vec3(roughness), 1.0);
    }

    if (visualize_ao)
    {
        FragColor = vec4(vec3(ao), 1.0);
    }

    if (visualize_world_position)
    {
        FragColor = vec4(fragPos / 100.0, 1.0);
    }
}
