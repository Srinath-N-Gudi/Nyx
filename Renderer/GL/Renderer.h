#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif

#include "VAO.h"
#include "IBO.h"

namespace Nyx {
    namespace Renderer {
        namespace GL {

            class Renderer {
            public:
                Renderer(VAO* vaoList, size_t vaoCount, GLenum drawMode);

                void draw();

            private:
                VAO* m_VAOs;
                size_t m_Count; // number of VAOs
                GLenum m_DrawMode;
            };

        } // namespace GL
    } // namespace Renderer
} // namespace Nyx
