#pragma once
#include <dvigl/core.h>
#include <dvigl/temp_math_3d.h>

using namespace std;

class Texture;

class SkinnedMesh {
public:
  SkinnedMesh();

  ~SkinnedMesh();

  bool LoadMesh(const string &Filename);

  void Render(glm::mat4 mvp);

  uint NumBones() const { return m_NumBones; }

  void BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms);

  float GetRunningTime();

  typedef std::list<GLuint> ShaderObjList;
  ShaderObjList m_shaderObjList;
  static const uint MAX_BONES = 100;
  long long m_frameTime;
  long long m_startTime;

  GLuint m_boneLocation[MAX_BONES];

  GLuint m_WVPLocation;
  GLuint m_colorTextureLocation;
  GLuint m_eyeWorldPosLocation;

private:
#define NUM_BONES_PER_VEREX 4

  struct BoneInfo {
    Matrix4f BoneOffset;
    Matrix4f FinalTransformation;

    BoneInfo() {
      BoneOffset.SetZero();
      FinalTransformation.SetZero();
    }
  };

  struct VertexBoneData {
    uint IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    VertexBoneData() { Reset(); };

    void Reset() {
      ZERO_MEM(IDs);
      ZERO_MEM(Weights);
    }

    void AddBoneData(uint BoneID, float Weight);
  };

  void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime,
                               const aiNodeAnim *pNodeAnim);
  void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime,
                                const aiNodeAnim *pNodeAnim);
  void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime,
                                const aiNodeAnim *pNodeAnim);
  uint FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);
  uint FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
  uint FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);
  const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation,
                                 const string NodeName);
  void ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode,
                         const Matrix4f &ParentTransform);
  bool InitFromScene(const aiScene *pScene, const string &Filename);
  void InitMesh(uint MeshIndex, const aiMesh *paiMesh,
                vector<Vector3f> &Positions, vector<Vector3f> &Normals,
                vector<Vector2f> &TexCoords, vector<VertexBoneData> &Bones,
                vector<unsigned int> &Indices);
  void LoadBones(uint MeshIndex, const aiMesh *paiMesh,
                 vector<VertexBoneData> &Bones);
  bool InitMaterials(const aiScene *pScene, const string &Filename);
  void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

  enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs
  };

  GLuint m_VAO;
  GLuint m_Buffers[NUM_VBs];

  struct MeshEntry {
    MeshEntry() {
      NumIndices = 0;
      BaseVertex = 0;
      BaseIndex = 0;
      MaterialIndex = INVALID_MATERIAL;
    }

    unsigned int NumIndices;
    unsigned int BaseVertex;
    unsigned int BaseIndex;
    unsigned int MaterialIndex;
  };

  vector<MeshEntry> m_Entries;
  vector<Texture *> m_Textures;

  map<string, uint> m_BoneMapping; // maps a bone name to its index
  uint m_NumBones;
  vector<BoneInfo> m_BoneInfo;
  Matrix4f m_GlobalInverseTransform;

  const aiScene *m_pScene;
};