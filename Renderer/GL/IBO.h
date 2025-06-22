#pragma once
namespace Nyx::Renderer::GL {

    class IBO {
    public:
        IBO();
        ~IBO();

        void data(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
        void bind() const;
        void unbind() const;

        GLuint getID() const { return m_ID; }

    private:
        GLuint m_ID;
    };

}
