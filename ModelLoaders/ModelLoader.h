#pragma once

#include "../NyxAPI.h"
#include "../vendor/assimp/Importer.hpp"
#include "../vendor/assimp/scene.h"
#include "../vendor/assimp/postprocess.h"
#include <string>
#include <vector>
#include <memory>
#include "../Renderer/GL/VAO.h"
#include "../Renderer/GL/VBO.h"
#include "../Renderer/GL/IBO.h"


namespace Nyx
{
    struct NYX_API Vertex
    {
        float Position[3];
        float Normal[3];
        float TexCoords[2];
        float Tangent[3];
        float Bitangent[3];
    };

    struct NYX_API Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int materialIndex;
    };

    struct NYX_API Material
    {
        std::string name;

        float diffuse[3]   = {1.0f, 1.0f, 1.0f};
        float specular[3]  = {1.0f, 1.0f, 1.0f};
        float ambient[3]   = {0.1f, 0.1f, 0.1f};

        std::string diffuseTex;
        std::string specularTex;
        std::string normalTex;
    };

        class NYX_API Model
        {
        public:
            Model(const std::string& path);
            ~Model() = default;

            const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
            const std::vector<Material>& GetMaterials() const { return m_Materials; }

            void LoadToVAO(
                size_t meshIndex,
                Renderer::GL::VBO& vbo,
                Renderer::GL::IBO& ibo,
                std::shared_ptr<Renderer::GL::VAO>& vao
            ) const;
            void LoadAsComplete(
                Renderer::GL::VBO& vbo,
                Renderer::GL::IBO& ibo,
                std::shared_ptr<Renderer::GL::VAO>& vao
            ) const;

        private:
            void LoadModel(const std::string& path);
            void ProcessNode(aiNode* node, const aiScene* scene);
            Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
            Material ProcessMaterial(aiMaterial* mat);

        private:
            std::vector<Mesh> m_Meshes;
            std::vector<Material> m_Materials;
            std::string m_Directory;
        };
}
