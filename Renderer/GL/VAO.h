#pragma once
#include "VBO.h"
#include "IBO.h"
#include <vector>


#ifdef NYX_USE_GLAD
#include <glad/glad.h>
#include "../../NyxAPI.h"
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
			struct NYX_API VertexAttribute {
				GLuint index;       // location in shader
				GLint size;         // number of components (e.g. 3 for vec3)
				GLenum type;        // GL_FLOAT, etc.
				GLboolean normalized;
				GLsizei stride;     // full vertex size
				size_t offset;      // offset to this attribute
			};



			class NYX_API VAO
			{
			private:
				GLuint m_VAO; 
				VBO* m_VBO;
				IBO* m_IBO;
				bool m_HIBO = false;
			public:
				VAO(VBO* vbo);
				~VAO();

				IBO* getIBO();

				void bind() const;
				void unbind() const;
				void setLayout(const std::vector<VertexAttribute>& layout);
				void attachIndexBuffer(IBO* ibo);
				inline GLuint getID() { return m_VAO; }
				inline bool hasIBO() { return m_HIBO;  }
				inline VBO* getVBO() { return m_VBO; }
			
			};


		}
	}
}