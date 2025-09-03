
#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#include "../../NyxAPI.h"
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif


#include <iostream>

namespace Nyx {
    namespace Renderer {
        namespace GL {
            struct NYX_API TextureParams {
                GLint wrapS = GL_REPEAT;
                GLint wrapT = GL_REPEAT;
                GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
                GLint magFilter = GL_LINEAR;
            };

            class NYX_API Texture2D {
            public:
                Texture2D();
                ~Texture2D();

                // Upload pixel data to GPU (expects raw RGBA/RGB data)
                void setTextureParams(const TextureParams& params = {});
                void setData(int width, int height, int channels, const void* data);
                // Bind to a texture unit (GL_TEXTURE0 + slot)
                void bind();
                void unbind();
                void ActivateTextureAtSlot(unsigned int slot);

                // Get OpenGL texture ID (if needed externally)
                GLuint id() const { return m_TextureID; }
            private:
                GLuint m_TextureID = 0;
            };

        }
    }
}