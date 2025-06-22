#include "VBO.h"	

namespace Nyx
{
	namespace Renderer
	{
		namespace GL
		{		
			VBO::VBO()
			{
				glGenBuffers(1, &m_VBO);
			}
			VBO::~VBO()
			{
			}
			void VBO::data(const void* data, GLsizeiptr size, GLenum usage)
			{
				this->bind();
				glBufferData(GL_ARRAY_BUFFER, size, data, usage);
				this->unbind();
			}
			void VBO::bind() const
			{ 
				glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			}
			void VBO::unbind() const
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
	}
}