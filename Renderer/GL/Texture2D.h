
#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif


#include <iostream>

namespace Nyx {
    namespace Renderer {
        namespace GL {
            struct TextureParams {
                GLint wrapS = GL_REPEAT;
                GLint wrapT = GL_REPEAT;
                GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
                GLint magFilter = GL_LINEAR;
            };

            class Texture2D {
            public:
                Texture2D(unsigned int slot=0);
                ~Texture2D();

                // Upload pixel data to GPU (expects raw RGBA/RGB data)
                void setData(int width, int height, int channels, const void* data, const TextureParams& params = {});

                // Bind to a texture unit (GL_TEXTURE0 + slot)
                void bind();
                void unbind() const;

                // Get OpenGL texture ID (if needed externally)
                GLuint id() const { return m_TextureID; }
                inline unsigned int getSlot() const { return m_Slot; }
            private:
                GLuint m_TextureID = 0;
                unsigned int m_Slot = 0;
            };

        }
    }
}