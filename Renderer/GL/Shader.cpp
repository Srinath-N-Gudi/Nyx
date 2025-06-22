#include "Shader.h"


namespace Nyx {
    namespace Renderer {
        namespace GL {

            Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
            {
                std::string vertexSrc = readFile(vertexPath);
                std::string fragmentSrc = readFile(fragmentPath);
                unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
                unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

                m_ShaderID = glCreateProgram();
                glAttachShader(m_ShaderID, vertexShader);
                glAttachShader(m_ShaderID, fragmentShader);
                glLinkProgram(m_ShaderID);

                int success;
                glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &success);
                if (!success) {
                    char infoLog[512];
                    glGetProgramInfoLog(m_ShaderID, 512, nullptr, infoLog);
                    std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
                }

                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }

            Shader::~Shader() {
                glDeleteProgram(m_ShaderID);
            }

            void Shader::bind() const { glUseProgram(m_ShaderID); }
            void Shader::unbind() const { glUseProgram(0); }

            std::string Shader::readFile(const std::string& path)
            {
                std::ifstream file(path);
                if (!file)
                    std::cerr << "Shader file not found: " << path << '\n';
                std::stringstream ss;
                ss << file.rdbuf();
                return ss.str();
            }

            unsigned int Shader::compileShader(unsigned int type, const std::string& source)
            {
                unsigned int id = glCreateShader(type);
                const char* src = source.c_str();
                glShaderSource(id, 1, &src, nullptr);
                glCompileShader(id);

                int success;
                glGetShaderiv(id, GL_COMPILE_STATUS, &success);
                if (!success) {
                    char infoLog[512];
                    glGetShaderInfoLog(id, 512, nullptr, infoLog);
                    std::cerr << "ERROR::SHADER::"
                        << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                        << "::COMPILATION_FAILED\n" << infoLog << std::endl;
                }

                return id;
            }

            int Shader::getUniformLocation(const std::string& name)
            {
                if (m_UniformLocationCache.contains(name))
                    return m_UniformLocationCache[name];

                int location = glGetUniformLocation(m_ShaderID, name.c_str());
                if (location == -1)
                    std::cerr << "Warning: uniform '" << name << "' doesn't exist.\n";

                m_UniformLocationCache[name] = location;
                return location;
            }

            void Shader::setUniform1i(const std::string& name, int value) {
                glUniform1i(getUniformLocation(name), value);
            }

            void Shader::setUniform1f(const std::string& name, float value) {
                glUniform1f(getUniformLocation(name), value);
            }

            void Shader::setUniform2f(const std::string& name, float x, float y) {
                glUniform2f(getUniformLocation(name), x, y);
            }

            void Shader::setUniform3f(const std::string& name, float x, float y, float z) {
                glUniform3f(getUniformLocation(name), x, y, z);
            }

            void Shader::setUniform4f(const std::string& name, float x, float y, float z, float w) {
                glUniform4f(getUniformLocation(name), x, y, z, w);
            }

            void Shader::setUniformMat4fv(const std::string& name, const float* matrix, bool transpose) {
                glUniformMatrix4fv(getUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, matrix);
            }

        }
    }
}