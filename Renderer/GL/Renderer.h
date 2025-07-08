#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif

#include <functional> // for std::function

#include "VAO.h"
#include "IBO.h"

namespace Nyx {
    namespace Renderer {
        namespace GL {

            class Renderer {
            public:
                using DrawCallback = std::function<void(int index, VAO* vao, void* userData, bool& skipDraw)>;

                Renderer(VAO** vaoList, size_t vaoCount, GLenum drawMode);

                void draw(DrawCallback callback = nullptr, void* userData = nullptr);

            private:
                VAO** m_VAOs;
                size_t m_Count; // number of VAOs
                GLenum m_DrawMode;

                DrawCallback m_Callback;
                void* m_UserData;
            };

        } // namespace GL
    } // namespace Renderer
} // namespace Nyx
