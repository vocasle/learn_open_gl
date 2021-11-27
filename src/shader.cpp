//
// Created by vocasle on 11/27/21.
//

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "utility.h"

struct ShaderSource {
    std::string vertex;
    std::string fragment;
};





static ShaderSource load_shader_source(const std::string &filename)
{
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream file(filename);
    std::string line;
    std::ostringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (std::getline(file, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}

unsigned int compile_shader(GLenum shader_type, const std::string &source)
{
    unsigned int shader = glCreateShader(shader_type);
    const char *src = source.c_str();
    GL_CALL(glShaderSource(shader, 1, &src, nullptr));
    GL_CALL(glCompileShader(shader));
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
        return 0;
    }
    return shader;
}

unsigned int compile_program(unsigned int vertex_shader, unsigned int fragment_shader)
{
    unsigned int program = glCreateProgram();
    GL_CALL(glAttachShader(program, vertex_shader));
    GL_CALL(glAttachShader(program, fragment_shader));
    GL_CALL(glLinkProgram(program));
    GL_CALL(glValidateProgram(program));
    GL_CALL(glDeleteShader(vertex_shader));
    GL_CALL(glDeleteShader(fragment_shader));

    int success;
    char info_log[512];
    GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success) {
        GL_CALL(glGetProgramInfoLog(program, 512, nullptr, info_log));
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
        return 0;
    }
    return program;
}

static unsigned int create_program(const ShaderSource &ss)
{
    return compile_program(compile_shader(GL_VERTEX_SHADER, ss.vertex),
                           compile_shader(GL_FRAGMENT_SHADER, ss.fragment));
}

Shader::Shader(const std::string &path)
{
   program_id = create_program(load_shader_source(path));
}

void Shader::use() const
{
    GL_CALL(glUseProgram(program_id));
}

void Shader::set_bool(const std::string &name, bool value) const
{
    GL_CALL(glUniform1i(glGetUniformLocation(program_id, name.c_str()), value));
}

void Shader::set_int(const std::string &name, int value) const
{
    GL_CALL(glUniform1i(glGetUniformLocation(program_id, name.c_str()), value));
}

void Shader::set_float(const std::string &name, float value) const
{
    GL_CALL(glUniform1f(glGetUniformLocation(program_id, name.c_str()), value));
}

Shader::~Shader()
{
    GL_CALL(glDeleteProgram(program_id));
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &vec) const
{
    GL_CALL(glUniform2fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(vec)));
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &vec) const
{
    GL_CALL(glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(vec)));
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &vec) const
{
    GL_CALL(glUniform4fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(vec)));
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)));
}

uint Shader::get_program() const
{
    return program_id;
}