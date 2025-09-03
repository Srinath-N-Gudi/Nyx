// Nyx/Renderer/GL/Texture2D.cpp
#include "Texture2D.h"



namespace Nyx {
    namespace Renderer {
        namespace GL {
            Texture2D::Texture2D() {
                glGenTextures(1, &m_TextureID);
            }

            Texture2D::~Texture2D() {
                glDeleteTextures(1, &m_TextureID);
            }
            void Texture2D::setTextureParams(const TextureParams& params) {
                glBindTexture(GL_TEXTURE_2D, m_TextureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);
            }
            void Texture2D::setData(int width, int height, int channels, const void* data, const TextureParams& params) {
                GLenum format = GL_RGB;
                if (channels == 4) format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            void Texture2D::bind(unsigned int slot){
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, m_TextureID);
            }

            void Texture2D::unbind(unsigned int slot) {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

        }
    }
}