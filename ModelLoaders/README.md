# Nyx ModelLoader - Easy 3D Model Loading with Assimp

A lightweight, easy-to-use C++ wrapper around Assimp for loading 3D models with PBR material support. Designed for modern OpenGL/Vulkan applications with zero dependencies except Assimp.

## Features

- 🚀 **Super Simple API** - Load any 3D model with just one line of code
- 📦 **Complete GLB/GLTF Support** - Handles embedded textures and materials perfectly
- 🎨 **PBR Materials** - Full physically-based rendering material properties
- ⚡ **Optimized** - Automatic texture caching, mesh optimization, and batching support
- 🔧 **No Dependencies** - Just Assimp (no GLM, no other math libraries required)
- 📐 **Built-in Math** - Includes Vec2, Vec3, Vec4, and Mat4 structures
- 🎯 **Production Ready** - Bounding boxes, error handling, and robust loading

## Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [Loading Models](#loading-models)
  - [Accessing Mesh Data](#accessing-mesh-data)
  - [Material System](#material-system)
  - [Texture Handling](#texture-handling)
- [Complete Examples](#complete-examples)
  - [OpenGL Integration](#opengl-integration)
  - [PBR Rendering Setup](#pbr-rendering-setup)
  - [Handling GLB Files](#handling-glb-files)
- [Data Structures](#data-structures)
- [Optimization Features](#optimization-features)
- [Troubleshooting](#troubleshooting)

## Installation

### Prerequisites
- C++17 or later
- Assimp library installed and linked
- OpenGL/Vulkan (for rendering, not required for loading)

### Setup
1. Copy `ModelLoader.h` and `ModelLoader.cpp` to your project
2. Include the header:
```cpp
#include "ModelLoader.h"
```
3. Link against Assimp in your CMake/build system

### CMake Example
```cmake
find_package(assimp REQUIRED)
target_link_libraries(your_project assimp)
```

## Quick Start

### Basic Usage
```cpp
#include "ModelLoader.h"

// Load a model (supports OBJ, FBX, GLTF, GLB, DAE, etc.)
auto model = Nyx::ModelLoader::LoadModel("assets/character.glb");

if (!model) {
    std::cerr << "Failed to load model!" << std::endl;
    return -1;
}

// Access meshes
for (const auto& mesh : model->GetMeshes()) {
    std::cout << "Mesh: " << mesh.name << std::endl;
    std::cout << "Vertices: " << mesh.vertices.size() << std::endl;
    std::cout << "Indices: " << mesh.indices.size() << std::endl;
}
```

## API Reference

### Loading Models

#### Basic Loading
```cpp
// Method 1: Simple loading with default flags
auto model = Nyx::ModelLoader::LoadModel("path/to/model.glb");

// Method 2: Using the Model class directly
Nyx::ModelLoader::Model model;
bool success = model.Load("path/to/model.fbx");

// Method 3: Custom Assimp flags
auto model = Nyx::ModelLoader::LoadModelWithFlags(
    "path/to/model.obj",
    aiProcess_Triangulate | aiProcess_GenNormals
);
```

#### Default Loading Flags
By default, models are loaded with these optimizations:
- `aiProcess_Triangulate` - Convert all faces to triangles
- `aiProcess_GenSmoothNormals` - Generate smooth normals if missing
- `aiProcess_FlipUVs` - Flip texture coordinates for OpenGL
- `aiProcess_CalcTangentSpace` - Calculate tangents and bitangents
- `aiProcess_JoinIdenticalVertices` - Merge duplicate vertices
- `aiProcess_OptimizeMeshes` - Optimize mesh data
- `aiProcess_OptimizeGraph` - Optimize scene hierarchy
- `aiProcess_FixInfacingNormals` - Fix inverted normals
- `aiProcess_GenBoundingBoxes` - Generate bounding boxes

### Accessing Mesh Data

#### Get All Meshes
```cpp
const std::vector<Nyx::ModelLoader::Mesh>& meshes = model->GetMeshes();

for (const auto& mesh : meshes) {
    // Process each mesh
}
```

#### Get Specific Mesh
```cpp
// By index
const auto* mesh = model->GetMesh(0);

// By name
const auto* mesh = model->GetMesh("character_head");

if (mesh) {
    // Use the mesh
}
```

#### Mesh Properties
```cpp
const auto& mesh = model->GetMeshes()[0];

// Vertex data (ready for GPU upload)
std::vector<Vertex> vertices = mesh.vertices;
std::vector<unsigned int> indices = mesh.indices;

// Each vertex contains:
// - position (Vec3)
// - normal (Vec3)
// - texCoords (Vec2)
// - tangent (Vec3)
// - bitangent (Vec3)
// - color (Vec4)

// Bounding box for culling
Vec3 minBounds = mesh.minBounds;
Vec3 maxBounds = mesh.maxBounds;

// Transform matrix (relative to model root)
Mat4 transform = mesh.transform;

// Material index
int materialIndex = mesh.materialIndex;
```

### Material System

#### Accessing Materials
```cpp
// Get all materials
const std::vector<Nyx::ModelLoader::Material>& materials = model->GetMaterials();

// Get specific material
const auto* material = model->GetMaterial(mesh.materialIndex);
```

#### PBR Material Properties
```cpp
const auto* mat = model->GetMaterial(0);

// Base PBR properties
Vec3 albedo = mat->albedo;           // Base color (default: white)
float metallic = mat->metallic;      // Metalness (0-1, default: 0)
float roughness = mat->roughness;    // Roughness (0-1, default: 1)
float ao = mat->ao;                  // Ambient occlusion (0-1, default: 1)

// Emissive
Vec3 emissive = mat->emissive;       // Emissive color
float emissiveStrength = mat->emissiveStrength;

// Additional properties
float opacity = mat->opacity;        // Transparency (0-1)
float normalScale = mat->normalScale;
float occlusionStrength = mat->occlusionStrength;

// Rendering flags
bool doubleSided = mat->doubleSided;
bool alphaBlend = mat->alphaBlend;

// Texture indices (-1 if not present)
int albedoTexture = mat->albedoTexture;
int normalTexture = mat->normalTexture;
int metallicTexture = mat->metallicTexture;
int roughnessTexture = mat->roughnessTexture;
int aoTexture = mat->aoTexture;
int emissiveTexture = mat->emissiveTexture;
int metallicRoughnessTexture = mat->metallicRoughnessTexture; // Combined texture
```

### Texture Handling

#### Accessing Textures
```cpp
// Get texture by index
auto texture = model->GetTexture(material->albedoTexture);

if (texture) {
    std::string path = texture->path;
    std::string type = texture->type; // "diffuse", "normal", etc.
    bool isEmbedded = texture->isEmbedded;
    size_t uniqueId = texture->id; // Unique ID for caching
}
```

#### Embedded Textures (GLB Files)
```cpp
auto texture = model->GetTexture(material->albedoTexture);

if (texture->isEmbedded) {
    if (texture->data.empty()) {
        // Compressed texture (PNG/JPG stored in memory)
        // Decode using stb_image or similar:
        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory(
            texture->data.data(),
            texture->data.size(),
            &width, &height, &channels, 4
        );
    } else {
        // Raw RGBA pixel data
        int width = texture->width;
        int height = texture->height;
        int channels = texture->channels;
        const unsigned char* pixels = texture->data.data();
    }
}
```

#### External Textures
```cpp
if (!texture->isEmbedded) {
    // Load from file path
    std::string fullPath = texture->path;
    // Use stb_image or your preferred image loader
}
```

## Complete Examples

### OpenGL Integration

#### Complete Rendering Setup
```cpp
#include "ModelLoader.h"
#include <GL/glew.h>

class ModelRenderer {
private:
    struct MeshGL {
        GLuint VAO, VBO, EBO;
        size_t indexCount;
        int materialIndex;
    };
    
    std::vector<MeshGL> meshes;
    std::unique_ptr<Nyx::ModelLoader::Model> model;
    
public:
    bool LoadModel(const std::string& path) {
        model = Nyx::ModelLoader::LoadModel(path);
        if (!model) return false;
        
        // Create OpenGL buffers for each mesh
        for (const auto& mesh : model->GetMeshes()) {
            MeshGL glMesh;
            glMesh.indexCount = mesh.indices.size();
            glMesh.materialIndex = mesh.materialIndex;
            
            // Generate buffers
            glGenVertexArrays(1, &glMesh.VAO);
            glGenBuffers(1, &glMesh.VBO);
            glGenBuffers(1, &glMesh.EBO);
            
            glBindVertexArray(glMesh.VAO);
            
            // Upload vertex data
            glBindBuffer(GL_ARRAY_BUFFER, glMesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, 
                mesh.vertices.size() * sizeof(Nyx::ModelLoader::Vertex),
                mesh.vertices.data(), GL_STATIC_DRAW);
            
            // Upload index data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                mesh.indices.size() * sizeof(unsigned int),
                mesh.indices.data(), GL_STATIC_DRAW);
            
            // Setup vertex attributes
            // Position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                sizeof(Nyx::ModelLoader::Vertex), (void*)0);
            glEnableVertexAttribArray(0);
            
            // Normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                sizeof(Nyx::ModelLoader::Vertex), 
                (void*)offsetof(Nyx::ModelLoader::Vertex, normal));
            glEnableVertexAttribArray(1);
            
            // Texture coordinates
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                sizeof(Nyx::ModelLoader::Vertex),
                (void*)offsetof(Nyx::ModelLoader::Vertex, texCoords));
            glEnableVertexAttribArray(2);
            
            // Tangent
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                sizeof(Nyx::ModelLoader::Vertex),
                (void*)offsetof(Nyx::ModelLoader::Vertex, tangent));
            glEnableVertexAttribArray(3);
            
            // Bitangent
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                sizeof(Nyx::ModelLoader::Vertex),
                (void*)offsetof(Nyx::ModelLoader::Vertex, bitangent));
            glEnableVertexAttribArray(4);
            
            // Vertex color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE,
                sizeof(Nyx::ModelLoader::Vertex),
                (void*)offsetof(Nyx::ModelLoader::Vertex, color));
            glEnableVertexAttribArray(5);
            
            glBindVertexArray(0);
            
            meshes.push_back(glMesh);
        }
        
        return true;
    }
    
    void Render(GLuint shaderProgram) {
        for (const auto& glMesh : meshes) {
            // Set material uniforms
            const auto* material = model->GetMaterial(glMesh.materialIndex);
            if (material) {
                glUniform3f(glGetUniformLocation(shaderProgram, "material.albedo"),
                    material->albedo.x, material->albedo.y, material->albedo.z);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.metallic"),
                    material->metallic);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.roughness"),
                    material->roughness);
                glUniform1f(glGetUniformLocation(shaderProgram, "material.ao"),
                    material->ao);
            }
            
            // Draw mesh
            glBindVertexArray(glMesh.VAO);
            glDrawElements(GL_TRIANGLES, glMesh.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
};
```

### PBR Rendering Setup

#### Complete PBR Material Setup
```cpp
class PBRRenderer {
private:
    std::unordered_map<size_t, GLuint> textureCache;
    
    GLuint LoadTextureGL(const std::shared_ptr<Nyx::ModelLoader::Texture>& texture) {
        // Check cache
        auto it = textureCache.find(texture->id);
        if (it != textureCache.end()) {
            return it->second;
        }
        
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        if (texture->isEmbedded) {
            if (!texture->data.empty()) {
                // Raw RGBA data
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    texture->width, texture->height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, texture->data.data());
            } else {
                // Compressed - decode first
                int width, height, channels;
                unsigned char* pixels = stbi_load_from_memory(
                    texture->data.data(), texture->data.size(),
                    &width, &height, &channels, 4
                );
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                
                stbi_image_free(pixels);
            }
        } else {
            // Load from file
            int width, height, channels;
            unsigned char* pixels = stbi_load(
                texture->path.c_str(), &width, &height, &channels, 4
            );
            
            if (pixels) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                stbi_image_free(pixels);
            }
        }
        
        // Set texture parameters
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Cache the texture
        textureCache[texture->id] = textureID;
        
        return textureID;
    }
    
public:
    void SetupPBRMaterial(GLuint shader, 
                         const Nyx::ModelLoader::Model* model,
                         const Nyx::ModelLoader::Material* material) {
        // Set base PBR values
        glUniform3f(glGetUniformLocation(shader, "material.albedo"),
            material->albedo.x, material->albedo.y, material->albedo.z);
        glUniform1f(glGetUniformLocation(shader, "material.metallic"),
            material->metallic);
        glUniform1f(glGetUniformLocation(shader, "material.roughness"),
            material->roughness);
        glUniform1f(glGetUniformLocation(shader, "material.ao"),
            material->ao);
        glUniform3f(glGetUniformLocation(shader, "material.emissive"),
            material->emissive.x, material->emissive.y, material->emissive.z);
        
        // Bind textures
        int textureUnit = 0;
        
        // Albedo map
        if (material->albedoTexture >= 0) {
            auto texture = model->GetTexture(material->albedoTexture);
            GLuint texID = LoadTextureGL(texture);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(shader, "material.albedoMap"), textureUnit);
            glUniform1i(glGetUniformLocation(shader, "material.hasAlbedoMap"), 1);
            textureUnit++;
        } else {
            glUniform1i(glGetUniformLocation(shader, "material.hasAlbedoMap"), 0);
        }
        
        // Normal map
        if (material->normalTexture >= 0) {
            auto texture = model->GetTexture(material->normalTexture);
            GLuint texID = LoadTextureGL(texture);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(shader, "material.normalMap"), textureUnit);
            glUniform1i(glGetUniformLocation(shader, "material.hasNormalMap"), 1);
            textureUnit++;
        } else {
            glUniform1i(glGetUniformLocation(shader, "material.hasNormalMap"), 0);
        }
        
        // Metallic-Roughness map (combined)
        if (material->metallicRoughnessTexture >= 0) {
            auto texture = model->GetTexture(material->metallicRoughnessTexture);
            GLuint texID = LoadTextureGL(texture);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(shader, "material.metallicRoughnessMap"), textureUnit);
            glUniform1i(glGetUniformLocation(shader, "material.hasMetallicRoughnessMap"), 1);
            textureUnit++;
        } else {
            glUniform1i(glGetUniformLocation(shader, "material.hasMetallicRoughnessMap"), 0);
            
            // Try separate metallic and roughness maps
            if (material->metallicTexture >= 0) {
                auto texture = model->GetTexture(material->metallicTexture);
                GLuint texID = LoadTextureGL(texture);
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(glGetUniformLocation(shader, "material.metallicMap"), textureUnit);
                glUniform1i(glGetUniformLocation(shader, "material.hasMetallicMap"), 1);
                textureUnit++;
            }
            
            if (material->roughnessTexture >= 0) {
                auto texture = model->GetTexture(material->roughnessTexture);
                GLuint texID = LoadTextureGL(texture);
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(glGetUniformLocation(shader, "material.roughnessMap"), textureUnit);
                glUniform1i(glGetUniformLocation(shader, "material.hasRoughnessMap"), 1);
                textureUnit++;
            }
        }
        
        // AO map
        if (material->aoTexture >= 0) {
            auto texture = model->GetTexture(material->aoTexture);
            GLuint texID = LoadTextureGL(texture);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(shader, "material.aoMap"), textureUnit);
            glUniform1i(glGetUniformLocation(shader, "material.hasAoMap"), 1);
            textureUnit++;
        } else {
            glUniform1i(glGetUniformLocation(shader, "material.hasAoMap"), 0);
        }
        
        // Emissive map
        if (material->emissiveTexture >= 0) {
            auto texture = model->GetTexture(material->emissiveTexture);
            GLuint texID = LoadTextureGL(texture);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(shader, "material.emissiveMap"), textureUnit);
            glUniform1i(glGetUniformLocation(shader, "material.hasEmissiveMap"), 1);
            textureUnit++;
        } else {
            glUniform1i(glGetUniformLocation(shader, "material.hasEmissiveMap"), 0);
        }
        
        // Set rendering flags
        if (material->doubleSided) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
        }
        
        if (material->alphaBlend) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
    }
};
```

### Handling GLB Files

#### Complete GLB Loading Example
```cpp
void LoadGLBModel() {
    // GLB files work out of the box with embedded textures
    auto model = Nyx::ModelLoader::LoadModel("assets/scene.glb");
    
    if (!model) {
        std::cerr << "Error: " << model->GetLastError() << std::endl;
        return;
    }
    
    std::cout << "Model loaded successfully!" << std::endl;
    std::cout << "Meshes: " << model->GetMeshCount() << std::endl;
    std::cout << "Materials: " << model->GetMaterialCount() << std::endl;
    std::cout << "Textures: " << model->GetTextureCount() << std::endl;
    
    // Check for embedded textures
    for (size_t i = 0; i < model->GetTextureCount(); i++) {
        auto texture = model->GetTexture(i);
        if (texture->isEmbedded) {
            std::cout << "Embedded texture: " << texture->type 
                     << " (size: " << texture->data.size() << " bytes)" << std::endl;
        }
    }
    
    // Process each mesh
    for (const auto& mesh : model->GetMeshes()) {
        std::cout << "\nMesh: " << mesh.name << std::endl;
        std::cout << "  Vertices: " << mesh.vertices.size() << std::endl;
        std::cout << "  Triangles: " << mesh.indices.size() / 3 << std::endl;
        
        if (mesh.materialIndex >= 0) {
            const auto* mat = model->GetMaterial(mesh.materialIndex);
            std::cout << "  Material: " << mat->name << std::endl;
            std::cout << "    Metallic: " << mat->metallic << std::endl;
            std::cout << "    Roughness: " << mat->roughness << std::endl;
        }
    }
}
```

## Data Structures

### Vertex Structure
```cpp
struct Vertex {
    Vec3 position;      // Vertex position in model space
    Vec3 normal;        // Vertex normal (normalized)
    Vec2 texCoords;     // UV texture coordinates
    Vec3 tangent;       // Tangent vector (for normal mapping)
    Vec3 bitangent;     // Bitangent vector (for normal mapping)
    Vec4 color;         // Vertex color (default: white)
};
```

### Math Structures
```cpp
// 2D Vector
struct Vec2 {
    float x, y;
};

// 3D Vector
struct Vec3 {
    float x, y, z;
};

// 4D Vector
struct Vec4 {
    float x, y, z, w;
};

// 4x4 Matrix (row-major)
struct Mat4 {
    float m[16];  // Identity by default
};
```

## Optimization Features

### Texture Caching
```cpp
// Textures are automatically cached globally
// Clear cache when needed (e.g., between scenes)
Nyx::ModelLoader::TextureCache::GetInstance().Clear();
```

### Frustum Culling
```cpp
// Use bounding boxes for frustum culling
auto model = Nyx::ModelLoader::LoadModel("large_scene.glb");

// Global bounds
Vec3 modelCenter = model->GetCenter();
float modelRadius = model->GetRadius();

// Per-mesh bounds
for (const auto& mesh : model->GetMeshes()) {
    Vec3 meshMin = mesh.minBounds;
    Vec3 meshMax = mesh.maxBounds;
    
    // Perform frustum culling test
    if (IsInFrustum(meshMin, meshMax)) {
        // Render mesh
    }
}
```

### Memory Management
```cpp
// Models use smart pointers - automatic cleanup
{
    auto model = Nyx::ModelLoader::LoadModel("model.glb");
    // Use model...
} // Automatically cleaned up

// Manual cleanup if needed
model->Clear();
```

## Troubleshooting

### Common Issues and Solutions

#### Model Fails to Load
```cpp
auto model = Nyx::ModelLoader::LoadModel("model.glb");
if (!model) {
    // Check detailed error
    std::cerr << "Error: " << model->GetLastError() << std::endl;
}
```

#### Textures Appear Flipped
- The loader automatically applies `aiProcess_FlipUVs` for OpenGL
- For Vulkan/DirectX, load without this flag:
```cpp
auto model = Nyx::ModelLoader::LoadModelWithFlags(
    "model.glb",
    aiProcess_Triangulate | aiProcess_GenNormals
    // Note: No aiProcess_FlipUVs
);
```

#### Missing Normals/Tangents
- Default flags generate these automatically
- For custom control:
```cpp
unsigned int flags = aiProcess_Triangulate;
if (needNormals) flags |= aiProcess_GenSmoothNormals;
if (needTangents) flags |= aiProcess_CalcTangentSpace;
auto model = Nyx::ModelLoader::LoadModelWithFlags("model.obj", flags);
```

#### Large Models Performance
```cpp
// Use optimization flags
auto model = Nyx::ModelLoader::LoadModelWithFlags(
    "huge_model.fbx",
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |  // Merge duplicate vertices
    aiProcess_OptimizeMeshes |         // Optimize for fewer draw calls
    aiProcess_OptimizeGraph |          // Optimize scene hierarchy
    aiProcess_RemoveRedundantMaterials // Remove unused materials
);
```

#### Embedded Textures Not Loading
```cpp
// Check if textures are actually embedded
auto texture = model->GetTexture(0);
if (texture->isEmbedded) {
    if (texture->data.empty()) {
        // Compressed format - needs decoding
        // Use stb_image_from_memory
    } else {
        // Raw pixel data - ready to use
        // texture->width, texture->height, texture->data
    }
}
```

## Supported Formats

The loader supports all formats that Assimp supports:
- **Common**: OBJ, FBX, GLTF, GLB, DAE (Collada), 3DS, BLEND
- **Game Engines**: Unity3D, Unreal
- **CAD**: STL, PLY, IFC, STEP
- **Others**: X, MS3D, MD2, MD3, MD5, B3D, and many more

## Performance Tips

1. **Use GLB over GLTF** - Everything is embedded, faster loading
2. **Enable optimization flags** - Reduces draw calls and memory usage
3. **Cache textures globally** - Automatic with the built-in cache
4. **Use bounding boxes** - Implement frustum culling
5. **Batch by material** - Sort meshes by material to reduce state changes

## License

This wrapper is provided as-is for use in your projects. Assimp is licensed under the BSD 3-Clause License.
