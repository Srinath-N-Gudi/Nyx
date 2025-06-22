#pragma once
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