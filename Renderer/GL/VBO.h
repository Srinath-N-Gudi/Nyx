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

			public:
				VBO();
				~VBO();
				void bind() const;
				void unbind() const;
				void data(const void* data, GLsizeiptr size, GLenum usage);
				inline GLuint getID() const { return m_VBO; }
			};
		
		
		
		
		
		
		}
	}

}