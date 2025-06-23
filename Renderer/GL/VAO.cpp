#include "VAO.h"


namespace Nyx
{
	namespace Renderer
	{
		namespace GL
		{
			VAO::VAO(const VBO& vbo) :
				m_VBO(vbo)
			{
				glGenVertexArrays(1, &m_VAO);
			}
			VAO::~VAO()
			{
			}
			void VAO::bind() const
			{
				glBindVertexArray(m_VAO);
			}
			void VAO::unbind() const
			{
				glBindVertexArray(0);
			}
			void VAO::setLayout(const std::vector<VertexAttribute>& layout)
			{
				this->bind(); 
				m_VBO.bind();

				for (const auto& attr : layout) {
					glEnableVertexAttribArray(attr.index);
					glVertexAttribPointer(
						attr.index,
						attr.size,
						attr.type,
						attr.normalized,
						attr.stride,
						reinterpret_cast<const void*>(attr.offset)
					);
				}

				m_VBO.unbind();
				this->unbind();
			}
			void VAO::attachIndexBuffer(const IBO& ibo)
			{
				this->bind();
				ibo.bind();   
				m_IBO = ibo;
				m_HIBO = true;
				this->unbind(); 
				ibo.unbind();
			}
			IBO* VAO::getIBO() {
				if (m_HIBO)
					return &m_IBO;
				else
					return nullptr;
			}
		}
	}
}