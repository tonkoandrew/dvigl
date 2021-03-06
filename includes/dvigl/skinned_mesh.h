#pragma once
#include <dvigl/core.h>

using namespace std;

class Material;
class Shader;

class SkinnedMesh
{
public:
    SkinnedMesh(const aiScene* pScene);

    ~SkinnedMesh();

    void draw(float time, Shader* s);
    void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);
    void release();

    vector<glm::mat4> prev_Transforms;
    float bounding_radius = 0.0f;

private:
#define NUM_BONES_PER_VEREX 4
    float TicksPerSecond;

    struct BoneInfo
    {
        glm::mat4 BoneOffset;
        glm::mat4 FinalTransformation;

        BoneInfo()
        {
            BoneOffset = glm::mat4(0.0f);
            FinalTransformation = glm::mat4(0.0f);
        }
    };

    struct VertexBoneData
    {
        GLuint IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        VertexBoneData() { Reset(); };

        void Reset()
        {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }

        void AddBoneData(GLuint BoneID, float Weight);
    };

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
    bool InitFromScene(const aiScene* pScene);
    void InitMesh(GLuint MeshIndex, const aiMesh* paiMesh, vector<glm::vec3>& Positions, vector<glm::vec3>& Normals,
        vector<glm::vec3>& Tangents, vector<glm::vec3>& BiTangents, vector<glm::vec2>& TexCoords,
        vector<VertexBoneData>& Bones, vector<unsigned int>& Indices);
    void LoadBones(GLuint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene);

#define INVALID_MATERIAL 0xFFFFFFFF

    enum VB_TYPES
    {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TANGENT_VB,
        BITANGENT_VB,
        TEXCOORD_VB,
        BONE_VB,
        NUM_VBs
    };

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry
    {
        MeshEntry()
        {
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
    vector<Material*> materials;

    map<string, GLuint> m_BoneMapping; // maps a bone name to its index
    GLuint m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    glm::mat4 m_GlobalInverseTransform;

    const aiScene* m_pScene;
};
