#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#include "../../NyxAPI.h"
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif

#include <functional> // for std::function
#include <memory>
#include "VAO.h"
#include "IBO.h"

namespace Nyx {
    namespace Renderer {
        namespace GL {

            class NYX_API Renderer {
            public:
                using DrawCallback = std::function<void(int index, VAO* vao, void* userData, bool& skipDraw)>;

                Renderer(GLenum drawMode) ;

				void draw(VAO** vaos, size_t vaoCount, DrawCallback callback = nullptr, void* userData = nullptr);
                void draw(std::shared_ptr<VAO>* vaos,size_t vaoCount, DrawCallback callback = nullptr, void* userData = nullptr);
            private:
                GLenum m_DrawMode;

                DrawCallback m_Callback;
                void* m_UserData;
            };

        } // namespace GL
    } // namespace Renderer
} // namespace Nyx
