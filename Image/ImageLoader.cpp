#include "ImageLoader.h"


namespace Nyx {
    namespace Image
    { 
        bool Loader::LoadToTexture(Nyx::Renderer::GL::Texture2D& texture, const std::string& path, bool flip)
        {
            
                int width, height, channels;
				stbi_set_flip_vertically_on_load(flip); // Flip the image vertically on load for opengl
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
