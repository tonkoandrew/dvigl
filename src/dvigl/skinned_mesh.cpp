#include <dvigl/material.h>
#include <dvigl/material_manager.h>
#include <dvigl/skinned_mesh.h>

#include <dvigl/shader.h>
#include <dvigl/shader_manager.h>

inline glm::mat4 aiMatrix4x4ToGlm(aiMatrix4x4* from)
{
    glm::mat4 to;

    to[0][0] = (GLfloat)from->a1;
    to[0][1] = (GLfloat)from->b1;
    to[0][2] = (GLfloat)from->c1;
    to[0][3] = (GLfloat)from->d1;
    to[1][0] = (GLfloat)from->a2;
    to[1][1] = (GLfloat)from->b2;
    to[1][2] = (GLfloat)from->c2;
    to[1][3] = (GLfloat)from->d2;
    to[2][0] = (GLfloat)from->a3;
    to[2][1] = (GLfloat)from->b3;
    to[2][2] = (GLfloat)from->c3;
    to[2][3] = (GLfloat)from->d3;
    to[3][0] = (GLfloat)from->a4;
    to[3][1] = (GLfloat)from->b4;
    to[3][2] = (GLfloat)from->c4;
    to[3][3] = (GLfloat)from->d4;

    return to;
}

inline glm::mat4 aiMatrix3x3ToGlm(aiMatrix3x3* from)
{
    glm::mat4 to;

    to[0][0] = (GLfloat)from->a1;
    to[0][1] = (GLfloat)from->b1;
    to[0][2] = (GLfloat)from->c1;
    to[0][3] = (GLfloat)0;
    to[1][0] = (GLfloat)from->a2;
    to[1][1] = (GLfloat)from->b2;
    to[1][2] = (GLfloat)from->c2;
    to[1][3] = (GLfloat)0;
    to[2][0] = (GLfloat)from->a3;
    to[2][1] = (GLfloat)from->b3;
    to[2][2] = (GLfloat)from->c3;
    to[2][3] = (GLfloat)0;
    to[3][0] = (GLfloat)0;
    to[3][1] = (GLfloat)0;
    to[3][2] = (GLfloat)0;
    to[3][3] = (GLfloat)1;

    return to;
}

void SkinnedMesh::VertexBoneData::AddBoneData(GLuint BoneID, float Weight)
{
    for (GLuint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++)
    {
        if (Weights[i] == 0.0)
        {
            IDs[i] = BoneID;
            Weights[i] = Weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    assert(0);
}

SkinnedMesh::SkinnedMesh(const aiScene* scene)
{
    m_VAO = 0;
    ZERO_MEM(m_Buffers);
    m_NumBones = 0;
    m_pScene = scene;

    // LOG("\n");
    LOG("SkinnedMesh IMPORTED %d MESHES\n", scene->mNumMeshes);
    LOG("SkinnedMesh IMPORTED %d ANIMATIONS\n", scene->mNumAnimations);

    TicksPerSecond
        = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    // TicksPerSecond /= 5.0f;

    m_startTime = SDL_GetTicks();
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;

    m_GlobalInverseTransform = glm::inverse(aiMatrix4x4ToGlm(&(scene->mRootNode->mTransformation)));
    Ret = InitFromScene(scene);

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}

SkinnedMesh::~SkinnedMesh() { release(); }

void SkinnedMesh::release()
{
    for (GLuint i = 0; i < materials.size(); i++)
    {
        SAFE_DELETE(materials[i]);
    }

    if (m_Buffers[0] != 0)
    {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool SkinnedMesh::InitFromScene(const aiScene* scene)
{
    m_Entries.resize(scene->mNumMeshes);
    materials.resize(scene->mNumMaterials);

    vector<glm::vec3> Positions;
    vector<glm::vec3> Normals;
    vector<glm::vec3> Tangents;
    vector<glm::vec3> BiTangents;
    vector<glm::vec2> TexCoords;
    vector<VertexBoneData> Bones;
    vector<GLuint> Indices;

    GLuint NumVertices = 0;
    GLuint NumIndices = 0;

    // Count the number of vertices and indices
    for (GLuint i = 0; i < m_Entries.size(); i++)
    {
        m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += scene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    Tangents.reserve(NumVertices);
    BiTangents.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
    // LOG("NumVertices = %d\n", NumVertices);

    // Initialize the meshes in the scene one by one
    for (GLuint i = 0; i < m_Entries.size(); i++)
    {
        const aiMesh* paiMesh = scene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, Tangents, BiTangents, TexCoords, Bones, Indices);
    }

    if (!InitMaterials(scene))
    {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_pos_loc);
    glVertexAttribPointer(attr_pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_texcoord_loc);
    glVertexAttribPointer(attr_texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_normal_loc);
    glVertexAttribPointer(attr_normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_tangent_loc);
    glVertexAttribPointer(attr_tangent_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BITANGENT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BiTangents[0]) * BiTangents.size(), &BiTangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_bitangent_loc);
    glVertexAttribPointer(attr_bitangent_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attr_bone_id_loc);
    glVertexAttribIPointer(attr_bone_id_loc, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(attr_bone_weight_loc);
    glVertexAttribPointer(attr_bone_weight_loc, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    for (GLuint i = 0; i < scene->mNumMeshes; i++)
    {
        delete scene->mMeshes[i];
    }

    Positions.clear();
    Normals.clear();
    Tangents.clear();
    BiTangents.clear();
    TexCoords.clear();
    Bones.clear();
    Indices.clear();

    return GLCheckError();
}

void SkinnedMesh::InitMesh(GLuint MeshIndex, const aiMesh* paiMesh, vector<glm::vec3>& Positions,
    vector<glm::vec3>& Normals, vector<glm::vec3>& Tangents, vector<glm::vec3>& BiTangents,
    vector<glm::vec2>& TexCoords, vector<VertexBoneData>& Bones, vector<GLuint>& Indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (GLuint i = 0; i < paiMesh->mNumVertices; i++)
    {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
        const aiVector3D* pBiTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mBitangents[i]) : &Zero3D;

        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
        Tangents.push_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
        BiTangents.push_back(glm::vec3(pBiTangent->x, pBiTangent->y, pBiTangent->z));
    }

    LoadBones(MeshIndex, paiMesh, Bones);

    // Populate the index buffer
    for (GLuint i = 0; i < paiMesh->mNumFaces; i++)
    {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

void SkinnedMesh::LoadBones(GLuint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (GLuint i = 0; i < pMesh->mNumBones; i++)
    {
        GLuint BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);

        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
        {
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(&(pMesh->mBones[i]->mOffsetMatrix));
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else
        {
            BoneIndex = m_BoneMapping[BoneName];
        }

        for (GLuint j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
        {
            GLuint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

bool SkinnedMesh::InitMaterials(const aiScene* scene)
{
    string Dir = "../res/materials";

    bool Ret = true;

    // Initialize the materials
    for (GLuint i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* pMaterial = scene->mMaterials[i];

        materials[i] = NULL;
        // m_NormalMaps[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                string p(Path.data);

                if (p.substr(0, 2) == ".\\")
                {
                    p = p.substr(2, p.size() - 2);
                }

                string FullPath = Dir + "/" + p;

                if (!MaterialMgr::ptr()->load_material(FullPath, FullPath))
                {
                    LOG("FAILED TO LOAD Texture");
                    return false;
                }

                materials[i] = MaterialMgr::ptr()->get_material(FullPath);
            }
        }
    }

    return Ret;
}

float SkinnedMesh::GetRunningTime()
{
    float RunningTime = (SDL_GetTicks() - m_startTime) / 1000.0f;
    return RunningTime;
}

void SkinnedMesh::draw()
{
    vector<glm::mat4> Transforms;

    float RunningTime = GetRunningTime();

    // LOG("RUNTIME %f\n", RunningTime);

    BoneTransform(RunningTime, Transforms);

    Shader* s = ShaderMgr::ptr()->get_shader("skinned_geometry");
    s->bind();
    s->uniformMatrix4("gBones", &Transforms);
    s->uniformMatrix4("prev_gBones", &prev_Transforms);

    glBindVertexArray(m_VAO);

    for (GLuint i = 0; i < m_Entries.size(); i++)
    {
        const GLuint MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < materials.size());

        if (materials[MaterialIndex])
        {
            materials[MaterialIndex]->bind();
        }

        glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT,
            (void*)(sizeof(GLuint) * m_Entries[i].BaseIndex), m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);

    prev_Transforms = Transforms;
}

GLuint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (GLuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);

    return 0;
}

GLuint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (GLuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);

    return 0;
}

GLuint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (GLuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);

    return 0;
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    GLuint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    GLuint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime
        = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    GLuint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    GLuint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime
        = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    GLuint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    GLuint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime
        = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

// recursive
void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    glm::mat4 NodeTransformation(aiMatrix4x4ToGlm((aiMatrix4x4*)&(pNode->mTransformation)));

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        glm::mat4 ScalingM = glm::scale(glm::mat4(1.0), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        aiMatrix3x3 rot = RotationQ.GetMatrix();
        glm::mat4 RotationM = aiMatrix3x3ToGlm(&rot);

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        glm::mat4 TranslationM = glm::translate(glm::mat4(1.0), glm::vec3(Translation.x, Translation.y, Translation.z));

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
    {
        GLuint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation
            = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (GLuint i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
{
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, glm::mat4(1.0f));

    Transforms.resize(m_NumBones);

    for (GLuint i = 0; i < m_NumBones; i++)
    {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (GLuint i = 0; i < pAnimation->mNumChannels; i++)
    {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName)
        {
            return pNodeAnim;
        }
    }

    return NULL;
}