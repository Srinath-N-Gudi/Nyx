#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Nyx {
    namespace ModelLoader {

        // Simple math structures (no GLM dependency)
        struct Vec2 {
            float x, y;
            Vec2() : x(0), y(0) {}
            Vec2(float x, float y) : x(x), y(y) {}
        };

        struct Vec3 {
            float x, y, z;
            Vec3() : x(0), y(0), z(0) {}
            Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
        };

        struct Vec4 {
            float x, y, z, w;
            Vec4() : x(0), y(0), z(0), w(1) {}
            Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        };

        struct Mat4 {
            float m[16];
            Mat4() {
                for (int i = 0; i < 16; i++) m[i] = 0;
                m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
            }
        };

        // Vertex structure with all common attributes
        struct Vertex {
            Vec3 position;
            Vec3 normal;
            Vec2 texCoords;
            Vec3 tangent;
            Vec3 bitangent;
            Vec4 color;

            Vertex() : color(1, 1, 1, 1) {}
        };

        // Texture information
        struct Texture {
            std::string path;           // File path or embedded texture identifier
            std::string type;           // "diffuse", "normal", "metallic", "roughness", etc.
            std::vector<unsigned char> data;  // For embedded textures (like in GLB)
            int width = 0;
            int height = 0;
            int channels = 0;
            bool isEmbedded = false;

            // Unique ID for caching
            size_t id;
        };

        // PBR Material structure
        struct Material {
            std::string name;

            // PBR properties
            Vec3 albedo = Vec3(1.0f, 1.0f, 1.0f);
            float metallic = 0.0f;
            float roughness = 1.0f;
            float ao = 1.0f;
            Vec3 emissive = Vec3(0.0f, 0.0f, 0.0f);
            float emissiveStrength = 1.0f;

            // Additional properties
            float opacity = 1.0f;
            float normalScale = 1.0f;
            float occlusionStrength = 1.0f;

            // Texture indices (index into Model's textures vector, -1 if not present)
            int albedoTexture = -1;
            int normalTexture = -1;
            int metallicTexture = -1;
            int roughnessTexture = -1;
            int aoTexture = -1;
            int emissiveTexture = -1;
            int metallicRoughnessTexture = -1;  // Combined texture (common in GLTF/GLB)

            // Rendering properties
            bool doubleSided = false;
            bool alphaBlend = false;
        };

        // Mesh structure
        struct Mesh {
            std::string name;
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            int materialIndex = -1;  // Index into Model's materials vector

            // Bounding box for culling
            Vec3 minBounds;
            Vec3 maxBounds;

            // Transform relative to model root
            Mat4 transform;
        };

        // Main Model class
        class Model {
        public:
            Model() = default;
            ~Model() = default;

            // Load model from file
            bool Load(const std::string& path, unsigned int flags = 0);

            // Easy access to data
            const std::vector<Mesh>& GetMeshes() const { return meshes; }
            const std::vector<Material>& GetMaterials() const { return materials; }
            const std::vector<std::shared_ptr<Texture>>& GetTextures() const { return textures; }

            // Get specific mesh/material
            const Mesh* GetMesh(size_t index) const;
            const Mesh* GetMesh(const std::string& name) const;
            const Material* GetMaterial(size_t index) const;
            const std::shared_ptr<Texture> GetTexture(size_t index) const;

            // Model info
            const std::string& GetPath() const { return modelPath; }
            size_t GetMeshCount() const { return meshes.size(); }
            size_t GetMaterialCount() const { return materials.size(); }
            size_t GetTextureCount() const { return textures.size(); }

            // Bounding box of entire model
            Vec3 GetMinBounds() const { return globalMinBounds; }
            Vec3 GetMaxBounds() const { return globalMaxBounds; }
            Vec3 GetCenter() const;
            float GetRadius() const;  // Bounding sphere radius

            // Clear all data
            void Clear();

            // Get loading errors
            const std::string& GetLastError() const { return lastError; }

        private:
            std::vector<Mesh> meshes;
            std::vector<Material> materials;
            std::vector<std::shared_ptr<Texture>> textures;

            std::string modelPath;
            std::string directory;
            std::string lastError;

            Vec3 globalMinBounds;
            Vec3 globalMaxBounds;

            // Assimp importer (kept for the lifetime of the model)
            std::unique_ptr<Assimp::Importer> importer;

            // Processing functions
            void ProcessNode(aiNode* node, const aiScene* scene, const Mat4& parentTransform);
            Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const Mat4& transform);
            void LoadMaterials(const aiScene* scene);
            int LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
            std::shared_ptr<Texture> LoadTexture(const std::string& path, const std::string& type);
            std::shared_ptr<Texture> LoadEmbeddedTexture(const aiTexture* aiTex, const std::string& type, int index);

            // Helper functions
            Mat4 ConvertMatrix(const aiMatrix4x4& from);
            void UpdateBounds(const Vec3& point);
        };

        // Global texture cache for optimization
        class TextureCache {
        public:
            static TextureCache& GetInstance();

            std::shared_ptr<Texture> GetTexture(const std::string& path);
            void AddTexture(const std::string& path, std::shared_ptr<Texture> texture);
            void Clear();

        private:
            TextureCache() = default;
            std::unordered_map<std::string, std::shared_ptr<Texture>> cache;
        };

        // Utility functions for easy loading
        std::unique_ptr<Model> LoadModel(const std::string& path);
        std::unique_ptr<Model> LoadModelWithFlags(const std::string& path, unsigned int flags);

    } // namespace ModelLoader
} // namespace Nyx