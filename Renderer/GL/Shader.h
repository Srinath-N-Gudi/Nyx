#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Nyx {
    namespace Renderer {
        namespace GL {

            class Shader {
            public:
                Shader(const std::string& vertexPath, const std::string& fragmentPath);
                ~Shader();

                void bind() const;
                void unbind() const;

                void setUniform1i(const std::string& name, int value);
                void setUniform1f(const std::string& name, float value);
                void setUniform2f(const std::string& name, float x, float y);
                void setUniform3f(const std::string& name, float x, float y, float z);
                void setUniform4f(const std::string& name, float x, float y, float z, float w);
                void setUniformMat4fv(const std::string& name, const float* matrix, bool transpose = false);

                unsigned int getID() const { return m_ShaderID; }

            private:
                unsigned int m_ShaderID;
                std::unordered_map<std::string, int> m_UniformLocationCache;

                std::string readFile(const std::string& path);
                unsigned int compileShader(unsigned int type, const std::string& source);
                int getUniformLocation(const std::string& name);
            };

        }
    }
}
