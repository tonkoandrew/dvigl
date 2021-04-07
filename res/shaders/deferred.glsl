#defshader vertex
#version 410 core

layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}

#defshader fragment
#version 410 core
out vec4 FragColor;

// GBuffer
uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D materialInfoTexture;
uniform sampler2D worldPosTexture;
uniform sampler2D velocityTexture;
uniform sampler2D shadowmapTexture;
uniform sampler2D ssaoTexture;


uniform int visualization_type;

uniform float uVelocityScale;
uniform float u_shadowBias;

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
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

#define MAX_SAMPLES 10

const float PI = 3.14159265359;
// IBL
uniform int reflectionProbeMipCount;
uniform bool computeIBL;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Lighting
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;
// uniform mat4 viewInverse;
// uniform mat4 projectionInverse;
uniform mat4 lightSpaceMatrix;

uniform ivec3 numDirPointSpotLights;

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

float sample_ShadowMap(vec2 uv)
{
    if (uv.x < 0.001)
        return 1.0;
    if (uv.x > 0.999)
        return 1.0;
    if (uv.y < 0.001)
        return 1.0;
    if (uv.y > 0.999)
        return 1.0;
    return texture(shadowmapTexture, uv).r;
}

float CalculateShadow(vec3 fragPos, vec3 normal, vec3 fragToLight)
{
    vec4 fragPosLightClipSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 ndcCoords = fragPosLightClipSpace.xyz / fragPosLightClipSpace.w;
    vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

    float shadow = 0.0;
    float currentDepth = depthmapCoords.z;

    // Add shadow bias to avoid shadow acne. However too much bias can cause peter panning
    // float u_shadowBias = 0.0001;

    // Perform Percentage Closer Filtering (PCF) in order to produce soft shadows
    vec2 texelSize = 1.0 / textureSize(shadowmapTexture, 0);

    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            vec2 uv = depthmapCoords.xy + (texelSize * vec2(x, y));
            float sampledDepthPCF = sample_ShadowMap(uv);
            shadow += currentDepth > sampledDepthPCF + u_shadowBias ? 1.0 : 0.0;
        }
    }
    // shadow /= 81.0;
    // shadow /= 49.0;
    shadow /= 9.0;

    if (currentDepth > 1.0)
        shadow = 0.0;
    // shadow = 0.0;
    return shadow;
}

// TODO: Need to also add multiple shadow support
vec3 CalculateDirectionalLightRadiance(
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 viewDir, vec3 baseReflectivity)
{
    vec3 directLightIrradiance = vec3(0.0);

    for (int i = 0; i < numDirPointSpotLights.x; ++i)
    {
        vec3 lightDir = normalize(-dirLights[i].direction);
        vec3 halfway = normalize(lightDir + viewDir);
        vec3 radiance = dirLights[i].intensity * dirLights[i].lightColour;

        // Cook-Torrance Specular BRDF calculations
        float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfway, viewDir), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, viewDir, lightDir, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(viewDir, normal), 0.1) * max(dot(lightDir, normal), 0.0)
            + 0.001; // Prevents any division by zero
        vec3 specular = numerator / denominator;

        // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
        vec3 diffuse = diffuseRatio * albedo / PI;

        // Add the light's radiance to the irradiance sum
        directLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, lightDir), 0.0)
            * (1.0 - CalculateShadow(fragPos, normal, lightDir));
        // directLightIrradiance += (1.0 - CalculateShadow(fragPos, normal, lightDir));

    }

    return directLightIrradiance;
}

vec3 CalculatePointLightRadiance(
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 viewDir, vec3 baseReflectivity)
{
    vec3 pointLightIrradiance = vec3(0.0);

    for (int i = 0; i < numDirPointSpotLights.y; ++i)
    {
        vec3 fragToLight = normalize(pointLights[i].position - fragPos);
        vec3 halfway = normalize(viewDir + fragToLight);
        float fragToLightDistance = length(pointLights[i].position - fragPos);

        // Attenuation calculation (based on Epic's UE4 falloff model)
        float d = fragToLightDistance / pointLights[i].attenuationRadius;
        float d2 = d * d;
        float d4 = d2 * d2;
        float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
        float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
        vec3 radiance = pointLights[i].intensity * pointLights[i].lightColour * attenuation;

        // Cook-Torrance Specular BRDF calculations
        float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfway, viewDir), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, viewDir, fragToLight, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(viewDir, normal), 0.1) * max(dot(fragToLight, normal), 0.0)
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
    vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 viewDir, vec3 baseReflectivity)
{
    vec3 spotLightIrradiance = vec3(0.0);

    for (int i = 0; i < numDirPointSpotLights.z; ++i)
    {
        vec3 fragToLight = normalize(spotLights[i].position - fragPos);
        vec3 halfway = normalize(viewDir + fragToLight);
        float fragToLightDistance = length(spotLights[i].position - fragPos);

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
        vec3 fresnel = FresnelSchlick(max(dot(halfway, viewDir), 0.0), baseReflectivity);
        float geometry = GeometrySmith(normal, viewDir, fragToLight, roughness);

        // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify
        // the diffuse lighting based on the metallic parameter
        vec3 specularRatio = fresnel;
        vec3 diffuseRatio = vec3(1.0) - specularRatio;
        diffuseRatio *= 1.0 - metallic;

        // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
        vec3 numerator = specularRatio * normalDistribution * geometry;
        float denominator = 4 * max(dot(viewDir, normal), 0.1) * max(dot(fragToLight, normal), 0.0)
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

#define VIS_NONE 0
#define VIS_ALBEDO 1
#define VIS_NORMALS 2
#define VIS_METALLNESS 3
#define VIS_ROUGHNESS 4
#define VIS_AMBIENT_OCCLUSION 5
#define VIS_WORLD_POSITIONS 6
#define VIS_VELOCITY 7
#define VIS_WIREFRAME 8


vec3 sample_WorldPos(vec2 uv)
{
    return texture(worldPosTexture, uv).xyz;
}

vec3 sample_Normal(vec2 uv)
{
    vec3 normal = texture(normalTexture, uv).rgb;
    if (length(normal) < 0.5)
        discard;
    normal = normalize(normal);
    return normal;
}

vec4 sample_Albedo(vec2 uv)
{
    return texture(albedoTexture, uv);
}

vec4 sample_MaterialInfo(vec2 uv)
{
    return texture(materialInfoTexture, uv);
}

void main()
{

    // =============+++++++++++++++++============
    vec2 texelSize = 1.0 / vec2(textureSize(albedoTexture, 0));
    vec2 uv = gl_FragCoord.xy * texelSize;

    switch (visualization_type)
    {
        case VIS_NONE:
            vec3 fragPos = sample_WorldPos(uv);
            vec3 normal = sample_Normal(uv);
            vec4 alb = sample_Albedo(uv);
            vec3 albedo = alb.rgb;

            vec4 matInfo = sample_MaterialInfo(uv);
            float metallic = matInfo.r;
            float roughness = matInfo.g;
            float materialAO = matInfo.b;

            float sceneAO = texture(ssaoTexture, uv).r;
            float ao = min(materialAO, sceneAO) * 0.02;

            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectionVec = reflect(-viewDir, normal);

            // Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse
            // (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
            vec3 baseReflectivity = vec3(0.04);
            baseReflectivity = mix(baseReflectivity, albedo, metallic);

            // Calculate per light radiance for all of the direct lighting
            vec3 directLightIrradiance = vec3(0.0);
            directLightIrradiance += CalculateDirectionalLightRadiance(
                albedo, normal, metallic, roughness, fragPos, viewDir, baseReflectivity);
            directLightIrradiance = clamp(directLightIrradiance, 0.0, 1.0);

            directLightIrradiance
                += CalculatePointLightRadiance(albedo, normal, metallic, roughness, fragPos, viewDir, baseReflectivity);
            directLightIrradiance = clamp(directLightIrradiance, 0.0, 1.0);

            directLightIrradiance
                += CalculateSpotLightRadiance(albedo, normal, metallic, roughness, fragPos, viewDir, baseReflectivity);
            directLightIrradiance = clamp(directLightIrradiance, 0.0, 1.0);

            // Calcualte ambient IBL for both diffuse and specular
            vec3 ambient = vec3(ao) * albedo;
            // if (computeIBL)
            // {
            //     vec3 specularRatio = FresnelSchlick(max(dot(normal, viewDir), 0.0), baseReflectivity);
            //     vec3 diffuseRatio = vec3(1.0) - specularRatio;
            //     diffuseRatio *= 1.0 - metallic;

            //     vec3 indirectDiffuse = texture(irradianceMap, normal).rgb * albedo * diffuseRatio;

            //     vec3 prefilterColour
            //         = textureLod(prefilterMap, reflectionVec, roughness * (reflectionProbeMipCount - 1)).rgb;
            //     vec2 brdfIntegration = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
            //     vec3 indirectSpecular = prefilterColour * (specularRatio * brdfIntegration.x + brdfIntegration.y);

            //     ambient = (indirectDiffuse + indirectSpecular) * ao;
            // }

            FragColor = vec4(ambient + directLightIrradiance, 1.0);
            break;
        case VIS_ALBEDO:
            FragColor = vec4(sample_Albedo(uv).rgb, 1.0);
            break;

        case VIS_NORMALS:
            FragColor = vec4(sample_Normal(uv), 1.0);
            break;

        case VIS_METALLNESS:
            FragColor = vec4(vec3(sample_MaterialInfo(uv).r), 1.0);
            break;

        case VIS_ROUGHNESS:
            FragColor = vec4(vec3(sample_MaterialInfo(uv).g), 1.0);
            break;

        case VIS_AMBIENT_OCCLUSION:
            FragColor = vec4(vec3(sample_MaterialInfo(uv).b), 1.0);
            break;

        case VIS_WORLD_POSITIONS:
            FragColor = vec4(sample_WorldPos(uv) / 100.0, 1.0);
            break;

        case VIS_VELOCITY:
            vec2 velocity = texture(velocityTexture, uv).rg;

            // velocity = velocity * 2.0
            velocity *= uVelocityScale;

            float speed = length(velocity / texelSize);
            // int nSamples = clamp(int(speed), 1, MAX_SAMPLES);
            // vec4 oResult = texture(albedoTexture, uv);
            // for (int i = 1; i < nSamples; ++i) {
            //     vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
            //     oResult += texture(albedoTexture, uv + offset);
            // }
            // oResult /= float(nSamples);
            // FragColor = vec4(oResult.rgb, 1.0);

            FragColor = vec4(vec3(speed*0.01), 1.0);
            break;

        case VIS_WIREFRAME:

            vec3 nn = texture(normalTexture, uv).rgb;
            if (length(nn) < 0.5)
                discard;
            FragColor = vec4(1.0);
            break;
    }
}
