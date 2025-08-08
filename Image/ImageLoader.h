#pragma once

#include <string>
#include <iostream>
#include "../vendor/stb_image.h"
#include "../Renderer/GL/Texture2D.h"
namespace Nyx {

    namespace Image {
        class Loader {
        public:
            static bool LoadToTexture(Nyx::Renderer::GL::Texture2D& texture,
                                      const std::string& path
				);
        };
    }
}
