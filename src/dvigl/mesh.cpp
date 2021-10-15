#include <dvigl/mesh.h>

Mesh::Mesh() { }

Mesh::Mesh(struct aiMesh* mesh)
{
    vertex_count = (int)(mesh->mNumFaces) * 3;
    vertex_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    normal_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    tangent_data = (float*)malloc(sizeof(float) * vertex_count * 3);
    bitangent_data = (float*)malloc(sizeof(float) * vertex_count * 3);
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
                int v_index = ((t * 3 + i) * 3);
                int n_index = ((t * 3 + i) * 3);
                int tcoord_index = ((t * 3 + i) * 2);
                if (mesh->mNormals != NULL)
                {
                    glm::vec3 normal = glm::normalize(
                        glm::vec3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z));
                    normal_data[n_index] = normal.x;
                    normal_data[n_index + 1] = normal.y;
                    normal_data[n_index + 2] = normal.z;

                    // normal_data[n_index] = mesh->mNormals[index].x;
                    // normal_data[n_index + 1] = mesh->mNormals[index].y;
                    // normal_data[n_index + 2] = mesh->mNormals[index].z;
                }
                else
                {
                    // Sphere normal
                    LOG("Generating Spherical normals");
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

                if (mesh->mBitangents != NULL)
                {
                    bitangent_data[n_index] = mesh->mBitangents[index].x;
                    bitangent_data[n_index + 1] = mesh->mBitangents[index].y;
                    bitangent_data[n_index + 2] = mesh->mBitangents[index].z;
                }

                if (mesh->mTextureCoords[0] != NULL)
                {
                    textureCoord_data[tcoord_index] = mesh->mTextureCoords[0][index].x;
                    textureCoord_data[tcoord_index + 1] = mesh->mTextureCoords[0][index].y;
                }

                auto vertex = mesh->mVertices[index];

                vertex_data[v_index] = vertex.x;
                vertex_data[v_index + 1] = vertex.y;
                vertex_data[v_index + 2] = vertex.z;

                bounding_radius = fmax(bounding_radius, glm::length(glm::vec3(vertex.x, vertex.y, vertex.z)));
            }
        }
    }
    genVAO();
    free(vertex_data);
    free(normal_data);
    free(tangent_data);
    free(bitangent_data);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3, vertex_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_pos_loc);
        glVertexAttribPointer(attr_pos_loc, 3, GL_FLOAT, 0, 0, 0);

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * 3, bitangent_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attr_bitangent_loc);
        glVertexAttribPointer(attr_bitangent_loc, 3, GL_FLOAT, 0, 0, 0);

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
