//
// Created by vocasle on 10/30/21.
//

#ifndef LEARN_OPEN_GL_SHADER_H
#define LEARN_OPEN_GL_SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <concepts>

class Shader {
public:
    uint program_id;

    Shader(const char* vertex_file_path, const char* fragment_file_path);

    ~Shader();

    void use() const;

    void set_bool(const std::string& name, bool value) const;

    void set_int(const std::string& name, int value) const;

    void set_float(const std::string& name, float value) const;
};

static void check_shader_compile_error(uint shader)
{
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}

static void check_program_link_error(uint program)
{
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
    }
}

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
{
    std::ifstream vertex_shader_file(vertex_file_path);
    std::ifstream fragment_shader_file(fragment_file_path);

    if (!vertex_shader_file || !fragment_shader_file) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return;
    }

    std::ostringstream vertex_shader_stream;
    vertex_shader_stream << vertex_shader_file.rdbuf();
    std::ostringstream fragment_shader_stream;
    fragment_shader_stream << fragment_shader_file.rdbuf();

    const std::string vertex_source_code = vertex_shader_stream.str();
    const std::string fragment_source_code = fragment_shader_stream.str();
    const char *vertex_source_cstr = vertex_source_code.c_str();
    const char *fragment_source_cstr = fragment_source_code.c_str();

    const uint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const uint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_source_cstr, nullptr);
    glCompileShader(vertex_shader);
    check_shader_compile_error(vertex_shader);

    glShaderSource(fragment_shader, 1, &fragment_source_cstr, nullptr);
    glCompileShader(fragment_shader);
    check_shader_compile_error(fragment_shader);

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    check_program_link_error(program_id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() const
{
    glUseProgram(program_id);
}

void Shader::set_bool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::set_int(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

Shader::~Shader()
{
    glDeleteProgram(program_id);
}

#endif //LEARN_OPEN_GL_SHADER_H
