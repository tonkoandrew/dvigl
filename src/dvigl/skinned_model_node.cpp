#include <dvigl/skinned_mesh.h>
#include <dvigl/skinned_model_node.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

SkinnedModelNode::SkinnedModelNode(char* content, int content_size, std::string format, float scale)
{
    const struct aiScene* scene;

    int flags = aiProcess_SplitLargeMeshes | aiProcess_LimitBoneWeights | aiProcess_SplitByBoneCount
        | aiProcess_GenSmoothNormals | aiProcess_SortByPType | aiProcess_CalcTangentSpace
        | aiProcess_RemoveRedundantMaterials | aiProcess_TransformUVCoords | aiProcess_ValidateDataStructure
        | aiProcess_ImproveCacheLocality | aiProcess_FindInvalidData | aiProcess_FindDegenerates
        | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipUVs
        | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale | 0;

    aiPropertyStore* store = aiCreatePropertyStore();
    aiSetImportPropertyFloat(store, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);

    int import_components = aiComponent_CAMERAS | aiComponent_LIGHTS | aiComponent_MATERIALS | aiComponent_COLORS;
    aiSetImportPropertyInteger(store, AI_CONFIG_PP_RVC_FLAGS, import_components);

    scene = aiImportFileFromMemoryWithProperties(content, content_size, flags, format.c_str(), store);
    aiReleasePropertyStore(store);

    if (!scene)
    {
        LOG("ERROR LOADING MODEL\n");
        return;
    }

    meshes.resize(1);
    meshes[0] = new SkinnedMesh(scene);

    bounding_radius = meshes[0]->bounding_radius;
}

void SkinnedModelNode::draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw();
    }
}

void SkinnedModelNode::release()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->release();
    }
}

float SkinnedModelNode::get_bounding_sphere_radius() { return bounding_radius; }
