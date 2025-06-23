#include "IBO.h"


namespace Nyx
{
    namespace Renderer
    {
        namespace GL{
   
            IBO::IBO()
            {
                glGenBuffers(1, &m_ID);
            }
            void IBO::data(const void* data, GLsizeiptr size, int dataTypeSize ,GLenum usage)
            {
                this->bind();
                m_ICount = size / dataTypeSize;
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
            }
            IBO::~IBO()
            {
                glDeleteBuffers(1, &m_ID);
            }
            void IBO::bind() const
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
            }
            void IBO::unbind() const
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

}
}
}