#include <dvigl/mesh.h>
#include <dvigl/model_node.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

ModelNode::ModelNode(char *content, int content_size, std::string format) {
  const struct aiScene *scene;
  scene = aiImportFileFromMemory(
      content, content_size,
      // aiProcessPreset_TargetRealtime_Quality |
      // aiProcessPreset_TargetRealtime_Fast |
      // aiProcess_SplitLargeMeshes | aiProcess_Triangulate |
      // aiProcess_GenSmoothNormals | aiProcess_SortByPType |
      //     aiProcess_CalcTangentSpace | aiProcess_RemoveRedundantMaterials |
      //     aiProcess_TransformUVCoords |
      //     aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
      //     aiProcess_ValidateDataStructure |

      //     aiProcess_ImproveCacheLocality |

      //     aiProcess_FindInvalidData | aiProcess_FindDegenerates |
      //     aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
      // 0,
      aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
          aiProcess_JoinIdenticalVertices,
      format.c_str());
  if (!scene) {
    LOG("ERROR LOADING MODEL\n");
    return;
  }
  meshes.resize(scene->mNumMeshes);
  LOG("\n");
  LOG("IMPORTED %d MESHES\n", scene->mNumMeshes);
  LOG("IMPORTED %d ANIMATIONS\n", scene->mNumAnimations);

  for (int m = 0; m < scene->mNumMeshes; m++) {
    for (int i = 0; i < (int)scene->mNumAnimations; i++) {
      // LOG("Animation %d name: \"%s\" duration: %2.2f ticks, TicksPerSecond: "
      //     "%2.2f\n"
      //     "NumMeshChannels: %d NumChannels: %d\n",
      //     i, scene->mAnimations[i]->mName.C_Str(),
      //     scene->mAnimations[i]->mDuration,
      //     scene->mAnimations[i]->mTicksPerSecond,
      //     scene->mAnimations[i]->mNumMeshChannels,
      //     scene->mAnimations[i]->mNumChannels);
    }
    for (int i = 0; i < (int)scene->mNumMeshes; i++) {
      // LOG("mesh %d have %d vertices\n", i, scene->mMeshes[0]->mNumVertices);
      // LOG("mesh %d Have %d bones\n", i, scene->mMeshes[0]->mNumBones);
    }
    if (scene->mNumMeshes == 0) {
      LOG("EMPTY MODEL WITHOUT MESHES\n");
      return;
    }
    for (int i = 0; i < (int)scene->mMeshes[0]->mNumBones; i++) {
      const char *name = scene->mMeshes[0]->mBones[i]->mName.C_Str();
      int num_weights = scene->mMeshes[0]->mBones[i]->mNumWeights;
      // LOG("Bone \"%s\" have %d weights\n", name, num_weights);
    }

    meshes[m] = new Mesh(scene->mMeshes[m]);
    meshes[m]->mat_idx = scene->mMeshes[m]->mMaterialIndex;
    // LOG("MATERIAL INDEX: %d\n", scene->mMeshes[m]->mMaterialIndex);
  }

  std::string Dir = "../res/textures";

  for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
    const aiMaterial *pMaterial = scene->mMaterials[i];

    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString Path;

      if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL,
                                NULL, NULL, NULL) == AI_SUCCESS) {
        std::string p(Path.data);

        if (p.substr(0, 2) == ".\\") {
          p = p.substr(2, p.size() - 2);
        }

        std::string FullPath = Dir + "/" + p;

        // LOG("Texture %d = %s\n", i, FullPath.c_str());

        if (!TextureMgr::ptr()->load_texture(FullPath, FullPath)) {
          LOG("FAILED TO LOAD Texture");
          return;
        }

        for (int m = 0; m < scene->mNumMeshes; m++) {
          if (i == meshes[m]->mat_idx) {
            meshes[m]->texture = TextureMgr::ptr()->get_texture(FullPath);
          }
        }
      }
    }
  }
}

ModelNode::ModelNode(int w, int h) {
  meshes.resize(1);
  meshes[0] = new Mesh(w, h);
}

void ModelNode::draw() {
  for (unsigned int i = 0; i < meshes.size(); i++) {
    meshes[i]->texture->bind();
    meshes[i]->draw();
  }
}

void ModelNode::release() {
  for (unsigned int i = 0; i < meshes.size(); i++) {
    meshes[i]->release();
  }
}
