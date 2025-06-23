#include "Renderer.h"

namespace Nyx {
    namespace Renderer {
        namespace GL {

            Renderer::Renderer(VAO* vaoList, size_t vaoCount, GLenum drawMode)
                : m_VAOs(vaoList), m_Count(vaoCount), m_DrawMode(drawMode) {
            }

            void Renderer::draw() {
                for (size_t i = 0; i < m_Count; ++i) {
                    VAO& vao = m_VAOs[i];
                    vao.bind();
                    if (vao.hasIBO()) {
                        glDrawElements(m_DrawMode, vao.getIBO()->getCount(), GL_UNSIGNED_INT, nullptr);
                    }
                    else {
                        glDrawArrays(m_DrawMode, 0, vao.getVBO()->getCount());
                    }
                }
            }

        } // namespace GL
    } // namespace Renderer
} // namespace Nyx
