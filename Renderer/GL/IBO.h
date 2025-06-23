#pragma once


#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif


namespace Nyx{
    namespace Renderer
    { 
        namespace GL {
    class IBO {
    public:
        IBO();
        ~IBO();

        void data(const void* data, GLsizeiptr size,
                   int dataTypeSize=sizeof(GLuint),
                GLenum usage = GL_STATIC_DRAW);
        void bind() const;
        void unbind() const;
        GLuint getID() const { return m_ID; }
        inline GLsizeiptr getCount() { return m_ICount;  }

    private:
        GLuint m_ID;
        GLsizeiptr m_ICount = 0;
    };

}

}
}