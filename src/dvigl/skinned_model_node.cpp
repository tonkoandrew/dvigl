#include <dvigl/skinned_mesh.h>
#include <dvigl/skinned_model_node.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

SkinnedModelNode::SkinnedModelNode(char* content, int content_size, std::string format, float scale)
{
    const struct aiScene* scene;

    int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_FindInvalidData | aiProcess_SplitByBoneCount | aiProcess_FlipUVs |
        // aiProcess_ConvertToLeftHanded |
        aiProcess_CalcTangentSpace | aiProcess_RemoveRedundantMaterials |

        aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale | 0;

    aiPropertyStore* store = aiCreatePropertyStore();
    aiSetImportPropertyFloat(store, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);
    scene = aiImportFileFromMemoryWithProperties(content, content_size, flags, format.c_str(), store);
    aiReleasePropertyStore(store);

    if (!scene) {
        LOG("ERROR LOADING MODEL\n");
        return;
    }

    meshes.resize(1);
    meshes[0] = new SkinnedMesh(scene);
}

void SkinnedModelNode::draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i]->draw();
    }
}

void SkinnedModelNode::release()
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i]->release();
    }
}
