#include <dvigl/model_node.h>
#include <dvigl/mesh.h>

ModelNode::ModelNode(char * content, int content_size)
{
    const struct aiScene * scene;
    scene = aiImportFileFromMemory(
        content, content_size,
        // aiProcessPreset_TargetRealtime_Quality |
        // aiProcessPreset_TargetRealtime_Fast |
        // aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_SortByPType |
        //     aiProcess_CalcTangentSpace | aiProcess_RemoveRedundantMaterials | aiProcess_TransformUVCoords |
        //     aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure |

        //     aiProcess_ImproveCacheLocality |

        //     aiProcess_FindInvalidData | aiProcess_FindDegenerates | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
            // 0,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices,
        "md5mesh");
    if (!scene)
    {
        LOG("ERROR LOADING MODEL\n");
        return;
    }
    LOG("\n");
    LOG("IMPORTED %d MESHES\n", scene->mNumMeshes);
    LOG("IMPORTED %d ANIMATIONS\n", scene->mNumAnimations);
    for (int i = 0; i < (int)scene->mNumAnimations; i++)
    {
        LOG("Animation %d name: \"%s\" duration: %2.2f ticks, TicksPerSecond: "
            "%2.2f\n"
            "NumMeshChannels: %d NumChannels: %d\n",
            i, scene->mAnimations[i]->mName.C_Str(), scene->mAnimations[i]->mDuration,
            scene->mAnimations[i]->mTicksPerSecond, scene->mAnimations[i]->mNumMeshChannels,
            scene->mAnimations[i]->mNumChannels);
    }
    for (int i = 0; i < (int)scene->mNumMeshes; i++)
    {
        LOG("mesh %d have %d vertices\n", i, scene->mMeshes[0]->mNumVertices);
        LOG("mesh %d Have %d bones\n", i, scene->mMeshes[0]->mNumBones);
    }
    if (scene->mNumMeshes == 0)
    {
        LOG("EMPTY MODEL WITHOUT MESHES\n");
        return;
    }
    for (int i = 0; i < (int)scene->mMeshes[0]->mNumBones; i++)
    {
        const char * name = scene->mMeshes[0]->mBones[i]->mName.C_Str();
        int num_weights = scene->mMeshes[0]->mBones[i]->mNumWeights;
        LOG("Bone \"%s\" have %d weights\n", name, num_weights);
    }

    m = new Mesh(scene->mMeshes[0]);
}

ModelNode::ModelNode(int w, int h) { m = new Mesh(w, h); }
void ModelNode::draw() { m->draw(); }
void ModelNode::release() { m->release(); }
