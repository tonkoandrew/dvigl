#include <dvigl/mesh.h>

Mesh::Mesh() {}
Mesh::Mesh(int w, int h)
{
    vertex_count = 6;
    vertex_data = (float*)malloc(sizeof(float) * vertex_count * 4);
    normal_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    tangent_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    textureCoord_data = (float*)malloc(sizeof(float) * vertex_count * 2);

    vertex_data[0] = 0;
    vertex_data[1] = 0;
    vertex_data[2] = 0;
    vertex_data[3] = 1.0f;

    vertex_data[4] = 0;
    vertex_data[5] = h;
    vertex_data[6] = 0;
    vertex_data[7] = 1.0f;

    vertex_data[8] = w;
    vertex_data[9] = h;
    vertex_data[10] = 0;
    vertex_data[11] = 1.0f;

    vertex_data[12] = 0;
    vertex_data[13] = 0;
    vertex_data[14] = 0;
    vertex_data[15] = 1.0f;

    vertex_data[16] = w;
    vertex_data[17] = h;
    vertex_data[18] = 0;
    vertex_data[19] = 1.0f;

    vertex_data[20] = w;
    vertex_data[21] = 0;
    vertex_data[22] = 0;
    vertex_data[23] = 1.0f;

    textureCoord_data[0] = 0;
    textureCoord_data[1] = 1;

    textureCoord_data[2] = 0;
    textureCoord_data[3] = 0;

    textureCoord_data[4] = 1;
    textureCoord_data[5] = 0;

    textureCoord_data[6] = 0;
    textureCoord_data[7] = 1;

    textureCoord_data[8] = 1;
    textureCoord_data[9] = 0;

    textureCoord_data[10] = 1;
    textureCoord_data[11] = 1;

    normal_data[0] = 0;
    normal_data[1] = 1;
    normal_data[2] = 0;

    normal_data[3] = 0;
    normal_data[4] = 1;
    normal_data[5] = 0;

    normal_data[6] = 0;
    normal_data[7] = 1;
    normal_data[8] = 0;

    normal_data[9] = 0;
    normal_data[10] = 1;
    normal_data[11] = 0;

    normal_data[12] = 0;
    normal_data[13] = 1;
    normal_data[14] = 0;

    normal_data[15] = 0;
    normal_data[16] = 1;
    normal_data[17] = 0;

    tangent_data[0] = 1;
    tangent_data[1] = 0;
    tangent_data[2] = 0;

    tangent_data[3] = 1;
    tangent_data[4] = 0;
    tangent_data[5] = 0;

    tangent_data[6] = 1;
    tangent_data[7] = 0;
    tangent_data[8] = 0;

    tangent_data[9] = 1;
    tangent_data[10] = 0;
    tangent_data[11] = 0;

    tangent_data[12] = 1;
    tangent_data[13] = 0;
    tangent_data[14] = 0;

    tangent_data[15] = 1;
    tangent_data[16] = 0;
    tangent_data[17] = 0;

    genVAO();
    free(vertex_data);
    free(normal_data);
    free(tangent_data);
    free(textureCoord_data);
}

Mesh::Mesh(struct aiMesh* mesh)
{
    // if(mesh->mNumBones > 0)
    // {
    //     scale = glm::vec3(10.0f);
    // }

    vertex_count = (int)(mesh->mNumFaces) * 3;
    vertex_data = (float*)malloc(sizeof(float) * vertex_count * 4);
    normal_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    tangent_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    textureCoord_data = (float*)malloc(sizeof(float) * vertex_count * 2);

    int num_triangles = (int)(mesh->mNumFaces);

    for (int t = 0; t < num_triangles; t++)
    {
        struct aiFace* face = &mesh->mFaces[t];
        for (int i = 0; i < 3; i++)
        {
            int index = face->mIndices[i];
            if (index >= (int)mesh->mNumVertices)
            {
                LOG("Mesh index %d out of range %d\n", index, mesh->mNumVertices);
            }
            else
            {
                int v_index = ((t * 3 + i) * 4);
                int n_index = ((t * 3 + i) * 3);
                int tcoord_index = ((t * 3 + i) * 2);
                if (mesh->mNormals != NULL)
                {
                    // glm::vec3 normal =
                    // glm::normalize(glm::vec3(mesh->mNormals[index].x,
                    // mesh->mNormals[index].y, mesh->mNormals[index].z));
                    // normal_data[n_index] = normal.x;
                    // normal_data[n_index+1] = normal.y;
                    // normal_data[n_index+2] = normal.z;

                    normal_data[n_index] = mesh->mNormals[index].x;
                    normal_data[n_index + 1] = mesh->mNormals[index].y;
                    normal_data[n_index + 2] = mesh->mNormals[index].z;
                }
                else
                {
                    // Sphere normal
                    glm::vec3 normal = glm::normalize(
                        glm::vec3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z));
                    normal_data[n_index] = normal.x;
                    normal_data[n_index + 1] = normal.y;
                    normal_data[n_index + 2] = normal.z;
                }

                if (mesh->mTangents != NULL)
                {
                    tangent_data[n_index] = mesh->mTangents[index].x;
                    tangent_data[n_index + 1] = mesh->mTangents[index].y;
                    tangent_data[n_index + 2] = mesh->mTangents[index].z;
                }

                if (mesh->mTextureCoords[0] != NULL)
                {
                    textureCoord_data[tcoord_index] = mesh->mTextureCoords[0][index].x;
                    textureCoord_data[tcoord_index + 1] = mesh->mTextureCoords[0][index].y;
                }

                vertex_data[v_index] = mesh->mVertices[index].x;
                vertex_data[v_index + 1] = mesh->mVertices[index].y;
                vertex_data[v_index + 2] = mesh->mVertices[index].z;
                vertex_data[v_index + 3] = 1.0f;
            }
        }
    }
    genVAO();
    free(vertex_data);
    free(normal_data);
    free(tangent_data);
    free(textureCoord_data);
}

void Mesh::genVAO()
{
    if (vertex_count > 0)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 4, vertex_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_pos_loc);
        glVertexAttribPointer(attr_pos_loc, 4, GL_FLOAT, 0, 0, 0);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3, normal_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_normal_loc);
        glVertexAttribPointer(attr_normal_loc, 3, GL_FLOAT, 0, 0, 0);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3, tangent_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_tangent_loc);
        glVertexAttribPointer(attr_tangent_loc, 3, GL_FLOAT, 0, 0, 0);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 2, textureCoord_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_texcoord_loc);
        glVertexAttribPointer(attr_texcoord_loc, 2, GL_FLOAT, 0, 0, 0);
        glBindVertexArray(0);
    }
}

void Mesh::draw()
{
    if (vertex_count > 0)
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }
}

void Mesh::release() { glDeleteVertexArrays(1, &vao); }
