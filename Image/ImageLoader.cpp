#include "ImageLoader.h"


namespace Nyx {
    namespace Image
    { 
        bool Loader::LoadToTexture(Nyx::Renderer::GL::Texture2D& texture, const std::string& path)
        {
            
                int width, height, channels;
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
                if (!data) {
                    std::cerr << "Failed to load texture from: " << path << "\n";
                    return false;
                }

                texture.setData(width, height, channels, data);
                stbi_image_free(data);
                return true;

        }

    }
}
