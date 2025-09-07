#include "ModelLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace Nyx {
    namespace ModelLoader {

        // Helper function to get file directory
        static std::string GetDirectory(const std::string& path) {
            size_t last = path.find_last_of("/\\");
            return (last != std::string::npos) ? path.substr(0, last) : ".";
        }

        // Model implementation
        bool Model::Load(const std::string& path, unsigned int flags) {
            Clear();

            modelPath = path;
            directory = GetDirectory(path);

            // Create importer
            importer = std::make_unique<Assimp::Importer>();

            // Default flags for optimal loading
            if (flags == 0) {
                flags = aiProcess_Triangulate |
                    aiProcess_GenSmoothNormals |
                    aiProcess_FlipUVs |
                    aiProcess_CalcTangentSpace |
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_OptimizeMeshes |
                    aiProcess_OptimizeGraph |
                    aiProcess_FixInfacingNormals |
                    aiProcess_GenBoundingBoxes;
            }

            // Load the model
            const aiScene* scene = importer->ReadFile(path, flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                lastError = "Assimp Error: " + std::string(importer->GetErrorString());
                return false;
            }

            // Initialize bounds
            globalMinBounds = Vec3(1e10f, 1e10f, 1e10f);
            globalMaxBounds = Vec3(-1e10f, -1e10f, -1e10f);

            // Load materials first (so meshes can reference them)
            LoadMaterials(scene);

            // Process the scene graph
            Mat4 identity;
            ProcessNode(scene->mRootNode, scene, identity);

            return true;
        }

        void Model::ProcessNode(aiNode* node, const aiScene* scene, const Mat4& parentTransform) {
            Mat4 nodeTransform = ConvertMatrix(node->mTransformation);

            // Combine with parent transform
            Mat4 transform;
            // Simple matrix multiplication (row-major)
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    transform.m[i * 4 + j] = 0;
                    for (int k = 0; k < 4; k++) {
                        transform.m[i * 4 + j] += parentTransform.m[i * 4 + k] * nodeTransform.m[k * 4 + j];
                    }
                }
            }

            // Process meshes in this node
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(ProcessMesh(mesh, scene, transform));
            }

            // Process children
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                ProcessNode(node->mChildren[i], scene, transform);
            }
        }

        Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const Mat4& transform) {
            Mesh result;
            result.name = mesh->mName.C_Str();
            result.transform = transform;

            // Initialize bounds
            result.minBounds = Vec3(1e10f, 1e10f, 1e10f);
            result.maxBounds = Vec3(-1e10f, -1e10f, -1e10f);

            // Process vertices
            result.vertices.reserve(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;

                // Position
                vertex.position.x = mesh->mVertices[i].x;
                vertex.position.y = mesh->mVertices[i].y;
                vertex.position.z = mesh->mVertices[i].z;

                // Update bounds
                result.minBounds.x = std::min(result.minBounds.x, vertex.position.x);
                result.minBounds.y = std::min(result.minBounds.y, vertex.position.y);
                result.minBounds.z = std::min(result.minBounds.z, vertex.position.z);
                result.maxBounds.x = std::max(result.maxBounds.x, vertex.position.x);
                result.maxBounds.y = std::max(result.maxBounds.y, vertex.position.y);
                result.maxBounds.z = std::max(result.maxBounds.z, vertex.position.z);

                // Normal
                if (mesh->HasNormals()) {
                    vertex.normal.x = mesh->mNormals[i].x;
                    vertex.normal.y = mesh->mNormals[i].y;
                    vertex.normal.z = mesh->mNormals[i].z;
                }

                // Texture coordinates
                if (mesh->mTextureCoords[0]) {
                    vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                    vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
                }

                // Tangent and bitangent
                if (mesh->HasTangentsAndBitangents()) {
                    vertex.tangent.x = mesh->mTangents[i].x;
                    vertex.tangent.y = mesh->mTangents[i].y;
                    vertex.tangent.z = mesh->mTangents[i].z;

                    vertex.bitangent.x = mesh->mBitangents[i].x;
                    vertex.bitangent.y = mesh->mBitangents[i].y;
                    vertex.bitangent.z = mesh->mBitangents[i].z;
                }

                // Vertex colors
                if (mesh->mColors[0]) {
                    vertex.color.x = mesh->mColors[0][i].r;
                    vertex.color.y = mesh->mColors[0][i].g;
                    vertex.color.z = mesh->mColors[0][i].b;
                    vertex.color.w = mesh->mColors[0][i].a;
                }

                result.vertices.push_back(vertex);
            }

            // Process indices
            result.indices.reserve(mesh->mNumFaces * 3);
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    result.indices.push_back(face.mIndices[j]);
                }
            }

            // Material index
            result.materialIndex = mesh->mMaterialIndex;

            // Update global bounds
            UpdateBounds(result.minBounds);
            UpdateBounds(result.maxBounds);

            return result;
        }

        void Model::LoadMaterials(const aiScene* scene) {
            materials.reserve(scene->mNumMaterials);

            for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
                aiMaterial* aiMat = scene->mMaterials[i];
                Material mat;

                // Get material name
                aiString name;
                aiMat->Get(AI_MATKEY_NAME, name);
                mat.name = name.C_Str();

                // Base color / Albedo
                aiColor3D color;
                if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                    mat.albedo = Vec3(color.r, color.g, color.b);
                }

                // Metallic (try multiple possible keys)
                float metallic = 0.0f;
                if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) != AI_SUCCESS) {
                    aiMat->Get(AI_MATKEY_REFLECTIVITY, metallic);
                }
                mat.metallic = metallic;

                // Roughness
                float roughness = 1.0f;
                if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) != AI_SUCCESS) {
                    float shininess = 0.0f;
                    if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
                        // Convert shininess to roughness (approximate)
                        roughness = 1.0f - (shininess / 100.0f);
                    }
                }
                mat.roughness = std::max(0.04f, roughness); // Minimum roughness for PBR

                // Emissive
                aiColor3D emissive;
                if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
                    mat.emissive = Vec3(emissive.r, emissive.g, emissive.b);
                }

                // Opacity
                float opacity = 1.0f;
                aiMat->Get(AI_MATKEY_OPACITY, opacity);
                mat.opacity = opacity;

                // Two-sided
                int twoSided = 0;
                aiMat->Get(AI_MATKEY_TWOSIDED, twoSided);
                mat.doubleSided = twoSided != 0;

                // Load textures

                // Diffuse/Albedo
                int texCount = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
                if (texCount > 0) {
                    mat.albedoTexture = LoadMaterialTextures(aiMat, aiTextureType_DIFFUSE, "diffuse");
                }

                // Try base color for GLTF/GLB
                if (mat.albedoTexture == -1) {
                    texCount = aiMat->GetTextureCount(aiTextureType_BASE_COLOR);
                    if (texCount > 0) {
                        mat.albedoTexture = LoadMaterialTextures(aiMat, aiTextureType_BASE_COLOR, "albedo");
                    }
                }

                // Normal map
                texCount = aiMat->GetTextureCount(aiTextureType_NORMALS);
                if (texCount > 0) {
                    mat.normalTexture = LoadMaterialTextures(aiMat, aiTextureType_NORMALS, "normal");
                }
                else {
                    texCount = aiMat->GetTextureCount(aiTextureType_HEIGHT);
                    if (texCount > 0) {
                        mat.normalTexture = LoadMaterialTextures(aiMat, aiTextureType_HEIGHT, "normal");
                    }
                }

                // Metallic
                texCount = aiMat->GetTextureCount(aiTextureType_METALNESS);
                if (texCount > 0) {
                    mat.metallicTexture = LoadMaterialTextures(aiMat, aiTextureType_METALNESS, "metallic");
                }

                // Roughness
                texCount = aiMat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
                if (texCount > 0) {
                    mat.roughnessTexture = LoadMaterialTextures(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");
                }

                // Combined metallic-roughness (common in GLTF/GLB)
                texCount = aiMat->GetTextureCount(aiTextureType_UNKNOWN);
                if (texCount > 0) {
                    // Check if it's a metallic-roughness texture
                    aiString path;
                    if (aiMat->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS) {
                        std::string pathStr = path.C_Str();
                        if (pathStr.find("metallicRoughness") != std::string::npos ||
                            pathStr.find("metallic-roughness") != std::string::npos) {
                            mat.metallicRoughnessTexture = LoadMaterialTextures(aiMat, aiTextureType_UNKNOWN, "metallicRoughness");
                        }
                    }
                }

                // Ambient occlusion
                texCount = aiMat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
                if (texCount > 0) {
                    mat.aoTexture = LoadMaterialTextures(aiMat, aiTextureType_AMBIENT_OCCLUSION, "ao");
                }
                else {
                    texCount = aiMat->GetTextureCount(aiTextureType_LIGHTMAP);
                    if (texCount > 0) {
                        mat.aoTexture = LoadMaterialTextures(aiMat, aiTextureType_LIGHTMAP, "ao");
                    }
                }

                // Emissive
                texCount = aiMat->GetTextureCount(aiTextureType_EMISSIVE);
                if (texCount > 0) {
                    mat.emissiveTexture = LoadMaterialTextures(aiMat, aiTextureType_EMISSIVE, "emissive");
                }

                materials.push_back(mat);
            }
        }

        int Model::LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName) {
            if (mat->GetTextureCount(type) == 0) return -1;

            aiString str;
            mat->GetTexture(type, 0, &str);
            std::string path = str.C_Str();

            // Check if it's an embedded texture (common in GLB)
            if (path[0] == '*') {
                // Embedded texture reference
                int index = std::atoi(path.c_str() + 1);
                const aiScene* scene = importer->GetScene();
                if (scene && index >= 0 && index < (int)scene->mNumTextures) {
                    auto texture = LoadEmbeddedTexture(scene->mTextures[index], typeName, index);
                    if (texture) {
                        textures.push_back(texture);
                        return textures.size() - 1;
                    }
                }
            }
            else {
                // External texture file
                std::string fullPath = directory + "/" + path;

                // Check cache first
                auto& cache = TextureCache::GetInstance();
                auto cached = cache.GetTexture(fullPath);

                if (cached) {
                    // Find if already in our textures vector
                    auto it = std::find(textures.begin(), textures.end(), cached);
                    if (it != textures.end()) {
                        return std::distance(textures.begin(), it);
                    }
                    else {
                        textures.push_back(cached);
                        return textures.size() - 1;
                    }
                }
                else {
                    auto texture = LoadTexture(fullPath, typeName);
                    if (texture) {
                        cache.AddTexture(fullPath, texture);
                        textures.push_back(texture);
                        return textures.size() - 1;
                    }
                }
            }

            return -1;
        }

        std::shared_ptr<Texture> Model::LoadTexture(const std::string& path, const std::string& type) {
            auto texture = std::make_shared<Texture>();
            texture->path = path;
            texture->type = type;
            texture->isEmbedded = false;

            // Generate unique ID based on path
            std::hash<std::string> hasher;
            texture->id = hasher(path);

            // Note: Actual texture loading would happen here
            // Since you want to handle VAO/VBO yourself, I'm just storing the path
            // You can use stb_image or similar to load the actual data when needed

            return texture;
        }

        std::shared_ptr<Texture> Model::LoadEmbeddedTexture(const aiTexture* aiTex, const std::string& type, int index) {
            auto texture = std::make_shared<Texture>();
            texture->type = type;
            texture->isEmbedded = true;
            texture->path = "*" + std::to_string(index);

            if (aiTex->mHeight == 0) {
                // Compressed texture (PNG, JPG, etc.)
                texture->data.resize(aiTex->mWidth);
                memcpy(texture->data.data(), aiTex->pcData, aiTex->mWidth);
            }
            else {
                // Uncompressed RGBA data
                texture->width = aiTex->mWidth;
                texture->height = aiTex->mHeight;
                texture->channels = 4;
                size_t size = texture->width * texture->height * 4;
                texture->data.resize(size);
                memcpy(texture->data.data(), aiTex->pcData, size);
            }

            // Generate unique ID
            std::hash<std::string> hasher;
            texture->id = hasher(texture->path + type);

            return texture;
        }

        Mat4 Model::ConvertMatrix(const aiMatrix4x4& from) {
            Mat4 to;
            // Assimp uses column-major, we use row-major
            to.m[0] = from.a1; to.m[1] = from.a2; to.m[2] = from.a3; to.m[3] = from.a4;
            to.m[4] = from.b1; to.m[5] = from.b2; to.m[6] = from.b3; to.m[7] = from.b4;
            to.m[8] = from.c1; to.m[9] = from.c2; to.m[10] = from.c3; to.m[11] = from.c4;
            to.m[12] = from.d1; to.m[13] = from.d2; to.m[14] = from.d3; to.m[15] = from.d4;
            return to;
        }

        void Model::UpdateBounds(const Vec3& point) {
            globalMinBounds.x = std::min(globalMinBounds.x, point.x);
            globalMinBounds.y = std::min(globalMinBounds.y, point.y);
            globalMinBounds.z = std::min(globalMinBounds.z, point.z);
            globalMaxBounds.x = std::max(globalMaxBounds.x, point.x);
            globalMaxBounds.y = std::max(globalMaxBounds.y, point.y);
            globalMaxBounds.z = std::max(globalMaxBounds.z, point.z);
        }

        const Mesh* Model::GetMesh(size_t index) const {
            if (index < meshes.size()) return &meshes[index];
            return nullptr;
        }

        const Mesh* Model::GetMesh(const std::string& name) const {
            for (const auto& mesh : meshes) {
                if (mesh.name == name) return &mesh;
            }
            return nullptr;
        }

        const Material* Model::GetMaterial(size_t index) const {
            if (index < materials.size()) return &materials[index];
            return nullptr;
        }

        const std::shared_ptr<Texture> Model::GetTexture(size_t index) const {
            if (index < textures.size()) return textures[index];
            return nullptr;
        }

        Vec3 Model::GetCenter() const {
            return Vec3(
                (globalMinBounds.x + globalMaxBounds.x) * 0.5f,
                (globalMinBounds.y + globalMaxBounds.y) * 0.5f,
                (globalMinBounds.z + globalMaxBounds.z) * 0.5f
            );
        }

        float Model::GetRadius() const {
            Vec3 size(
                globalMaxBounds.x - globalMinBounds.x,
                globalMaxBounds.y - globalMinBounds.y,
                globalMaxBounds.z - globalMinBounds.z
            );
            return std::sqrt(size.x * size.x + size.y * size.y + size.z * size.z) * 0.5f;
        }

        void Model::Clear() {
            meshes.clear();
            materials.clear();
            textures.clear();
            modelPath.clear();
            directory.clear();
            lastError.clear();
        }

        // TextureCache implementation
        TextureCache& TextureCache::GetInstance() {
            static TextureCache instance;
            return instance;
        }

        std::shared_ptr<Texture> TextureCache::GetTexture(const std::string& path) {
            auto it = cache.find(path);
            if (it != cache.end()) {
                return it->second;
            }
            return nullptr;
        }

        void TextureCache::AddTexture(const std::string& path, std::shared_ptr<Texture> texture) {
            cache[path] = texture;
        }

        void TextureCache::Clear() {
            cache.clear();
        }

        // Utility functions
        std::unique_ptr<Model> LoadModel(const std::string& path) {
            auto model = std::make_unique<Model>();
            if (!model->Load(path)) {
                std::cerr << "Failed to load model: " << model->GetLastError() << std::endl;
                return nullptr;
            }
            return model;
        }

        std::unique_ptr<Model> LoadModelWithFlags(const std::string& path, unsigned int flags) {
            auto model = std::make_unique<Model>();
            if (!model->Load(path, flags)) {
                std::cerr << "Failed to load model: " << model->GetLastError() << std::endl;
                return nullptr;
            }
            return model;
        }

    } // namespace ModelLoader
} // namespace Nyx