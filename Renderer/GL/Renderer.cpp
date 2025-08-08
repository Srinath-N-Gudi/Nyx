#include "Renderer.h"
namespace Nyx {
    namespace Renderer {
        namespace GL {

            Renderer::Renderer(GLenum drawMode)
                : m_DrawMode(drawMode)
            {}
            void Renderer::draw(VAO** vaos, size_t vaoCount,DrawCallback callback, void* userData) {
                for (size_t i = 0; i < vaoCount; ++i) {
                    VAO* vao = vaos[i];

                    bool skipDraw = false;
                    if (callback) {
                        callback(static_cast<int>(i), vao, m_UserData, skipDraw);
                    }

                    if (skipDraw) continue;

                    vao->bind();
                    if (vao->hasIBO()) {
                        glDrawElements(m_DrawMode, vao->getIBO()->getCount(), GL_UNSIGNED_INT, nullptr);
                    }
                    else {
                        glDrawArrays(m_DrawMode, 0, vao->getVBO()->getCount());
                    }
                }
            }
			void Renderer::draw(std::shared_ptr<VAO>* vaos, size_t vaoCount, DrawCallback callback, void* userData) {
				for (size_t i = 0; i < vaoCount; ++i) {
					std::shared_ptr<VAO> vao = vaos[i];
					bool skipDraw = false;
					if (callback) {
						callback(static_cast<int>(i), vao.get(), m_UserData, skipDraw);
					}
					if (skipDraw) continue;
					vao->bind();
					if (vao->hasIBO()) {
						glDrawElements(m_DrawMode, vao->getIBO()->getCount(), GL_UNSIGNED_INT, nullptr);
					}
					else {
						glDrawArrays(m_DrawMode, 0, vao->getVBO()->getCount());
					}
				}
			}
        } // namespace GL
    } // namespace Renderer
} // namespace Nyx
