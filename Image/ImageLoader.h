#pragma once

#include <string>
#include <iostream>
#include "../vendor/stb_image.h"
#include "../Renderer/GL/Texture2D.h"
#include "../NyxAPI.h"
namespace Nyx {

    namespace Image {
        class NYX_API Loader {
        public:
            static bool LoadToTexture(Nyx::Renderer::GL::Texture2D& texture,
                                      const std::string& path, bool flip = true
				);
        };
    }
}
