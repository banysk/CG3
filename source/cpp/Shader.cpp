#ifndef SHADER_OPENGL
#define SHADER_OPENGL

    #ifndef GLM_OPENGL
    #define GLM_OPENGL
    #include "../../3rd/glm/glm/glm.hpp"
    #endif // !GLM_OPENGL

#include "../../utils/shaderLoader.cpp"

class Shader {
public:
    Shader(std::string vertex_path, std::string fragment_path) {
        shader_id = LoadShaders(vertex_path, fragment_path);
    }

    void use() const {
        glUseProgram(shader_id);
    }

    void set_mat4(const std::string& name, const glm::mat4& data) const {
        glUniformMatrix4fv(glGetUniformLocation(shader_id, &name[0]), 1, GL_FALSE, &data[0][0]);
    }

    void set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(shader_id, &name[0]), 1, &value[0]);
    }

    void set_vec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(shader_id, &name[0]), x, y, z);
    }

    void set_float(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(shader_id, &name[0]), value);
    }

    void set_int(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(shader_id, &name[0]), value);
    }

    GLuint shader_id;
};
#endif // !SHADER_OPENGL