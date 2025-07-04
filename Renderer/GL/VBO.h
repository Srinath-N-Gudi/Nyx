#pragma once

#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#elif defined(NYX_USE_GLEW)
#include <GL/glew.h>
#else
#error "No OpenGL loader defined. Define NYX_USE_GLAD or NYX_USE_GLEW before including Nyx headers."
#endif


namespace Nyx
{
	namespace Renderer
	{
		namespace GL
		{
		
			class VBO
			{
			private:
				GLuint m_VBO;
				GLsizeiptr m_TCount = 0;

			public:
				VBO();
				~VBO();
				
				void bind() const;
				void unbind() const;
				void data(const void* data, GLsizeiptr size, GLsizeiptr dataTypeValue=sizeof(GL_FLOAT), GLenum usage = GL_STATIC_DRAW);
				inline GLuint getID() const { return m_VBO; }
				inline GLsizeiptr getCount() const { return m_TCount; }
			};
		
		
		
		
		
		
		}
	}

}