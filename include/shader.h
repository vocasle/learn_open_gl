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

#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& vertex_path, const std::string& pixel_path);
    ~Shader();
    void use() const;
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec2(const std::string &name, const glm::vec2 &vec) const;
    void set_vec3(const std::string &name, const glm::vec3 &vec) const;
    void set_vec4(const std::string &name, const glm::vec4 &vec) const;
    void set_mat4(const std::string &name, const glm::mat4 &mat) const;
    unsigned int get_program() const;

    void set_mat3(const std::string& name, const glm::mat3 mat) const;

private:
    unsigned int program_id;
};

#endif //LEARN_OPEN_GL_SHADER_H
