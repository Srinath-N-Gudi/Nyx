#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <memory>
namespace Nyx
{
    Model::Model(const std::string& path)
    {
        LoadModel(path);
    }

    void Model::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_LimitBoneWeights);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        m_Directory = path.substr(0, path.find_last_of('/'));

        // Load materials first
        m_Materials.reserve(scene->mNumMaterials);
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            m_Materials.push_back(ProcessMaterial(scene->mMaterials[i]));
        }

        // Process nodes & meshes
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* /*scene*/)
    {
        Mesh outMesh;

        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex = {};

            if (mesh->HasPositions())
            {
                vertex.Position[0] = mesh->mVertices[i].x;
                vertex.Position[1] = mesh->mVertices[i].y;
                vertex.Position[2] = mesh->mVertices[i].z;
            }

            if (mesh->HasNormals())
            {
                vertex.Normal[0] = mesh->mNormals[i].x;
                vertex.Normal[1] = mesh->mNormals[i].y;
                vertex.Normal[2] = mesh->mNormals[i].z;
            }

            if (mesh->HasTextureCoords(0))
            {
                vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;
            }

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.Tangent[0] = mesh->mTangents[i].x;
                vertex.Tangent[1] = mesh->mTangents[i].y;
                vertex.Tangent[2] = mesh->mTangents[i].z;

                vertex.Bitangent[0] = mesh->mBitangents[i].x;
                vertex.Bitangent[1] = mesh->mBitangents[i].y;
                vertex.Bitangent[2] = mesh->mBitangents[i].z;
            }

            outMesh.vertices.push_back(vertex);
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                outMesh.indices.push_back(face.mIndices[j]);
        }

        // Material index
        outMesh.materialIndex = mesh->mMaterialIndex;

        return outMesh;
    }

    Material Model::ProcessMaterial(aiMaterial* mat)
    {
        Material material;

        aiString name;
        if (mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
            material.name = name.C_Str();

        aiColor3D color;
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            material.diffuse[0] = color.r, material.diffuse[1] = color.g, material.diffuse[2] = color.b;
        if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
            material.specular[0] = color.r, material.specular[1] = color.g, material.specular[2] = color.b;
        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
            material.ambient[0] = color.r, material.ambient[1] = color.g, material.ambient[2] = color.b;

        auto loadTex = [&](aiTextureType type) -> std::string {
            if (mat->GetTextureCount(type) > 0)
            {
                aiString str;
                mat->GetTexture(type, 0, &str);
                return str.C_Str();
            }
            return "";
            };

        material.diffuseTex = loadTex(aiTextureType_DIFFUSE);
        material.specularTex = loadTex(aiTextureType_SPECULAR);
        material.normalTex = loadTex(aiTextureType_NORMALS);

        return material;
    }
    void Model::LoadToVAO(
        size_t meshIndex,
        Nyx::Renderer::GL::VBO& vbo,
        Nyx::Renderer::GL::IBO& ibo,
        std::shared_ptr<Nyx::Renderer::GL::VAO>& vao
    ) const
    {
        if (meshIndex >= m_Meshes.size())
        {
            std::cerr << "Invalid mesh index: " << meshIndex << std::endl;
            return;
        }

        const Mesh& mesh = m_Meshes[meshIndex];

        // --- VBO ---
        vbo.data(
            mesh.vertices.data(),
            mesh.vertices.size() * sizeof(Vertex),
            GL_STATIC_DRAW
        );

        // --- IBO ---
        ibo.data(
            mesh.indices.data(),
            mesh.indices.size() * sizeof(unsigned int),
            sizeof(unsigned int),
            GL_STATIC_DRAW
        );

        // --- VAO ---
        vao = std::make_shared<Nyx::Renderer::GL::VAO>(mesh.indices.size());
        vao->addVBO(&vbo);
        vao->attachIndexBuffer(&ibo);

        vao->bind();
        GLsizei stride = sizeof(Vertex);

        vao->setLayout({
            { 0, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Position)   }, // Pos
            { 1, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Normal)     }, // Normal
            { 2, 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, TexCoords)  }, // UV
            { 3, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Tangent)    }, // Tangent
            { 4, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Bitangent)  }  // Bitangent
            });

        vao->unbind();
    }
    void Model::LoadAsComplete(
        Nyx::Renderer::GL::VBO& vbo,
        Nyx::Renderer::GL::IBO& ibo,
        std::shared_ptr<Nyx::Renderer::GL::VAO>& vao
    ) const
    {
        if (m_Meshes.empty())
        {
            std::cerr << "No meshes to combine in model.\n";
            return;
        }

        std::vector<Vertex> combinedVertices;
        std::vector<unsigned int> combinedIndices;

        size_t vertexOffset = 0;
        for (const auto& mesh : m_Meshes)
        {
            // append vertices
            combinedVertices.insert(combinedVertices.end(),
                mesh.vertices.begin(),
                mesh.vertices.end());

            // append indices (offset them!)
            for (unsigned int idx : mesh.indices)
                combinedIndices.push_back(idx + static_cast<unsigned int>(vertexOffset));

            vertexOffset += mesh.vertices.size();
        }

        // --- VBO ---
        vbo.data(
            combinedVertices.data(),
            combinedVertices.size() * sizeof(Vertex),
            GL_STATIC_DRAW
        );

        // --- IBO ---
        ibo.data(
            combinedIndices.data(),
            combinedIndices.size() * sizeof(unsigned int),
            sizeof(unsigned int),
            GL_STATIC_DRAW
        );

        // --- VAO ---
        vao = std::make_shared<Nyx::Renderer::GL::VAO>(combinedIndices.size());
        vao->addVBO(&vbo);
        vao->attachIndexBuffer(&ibo);

        vao->bind();
        GLsizei stride = sizeof(Vertex);

        vao->setLayout({
            { 0, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Position)   },
            { 1, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Normal)     },
            { 2, 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, TexCoords)  },
            { 3, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Tangent)    },
            { 4, 3, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, Bitangent)  }
            });

        vao->unbind();
    }
}
