//
// Created by vocasle on 11/27/21.
//
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "utility.h"

void gl_clear_error()
{
    while (glGetError()!=GL_NO_ERROR);
}

bool gl_log_call(const std::string& function_name, const std::string& filename, unsigned int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "ERROR: in "
                  << function_name << " ("
                  << error << ") "
                  << filename << ":"
                  << line
                  << std::endl;
        return false;
    }
    return true;
}

glm::vec3 calc_normal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    glm::vec3 u = b-a;
    glm::vec3 v = c-a;
    glm::vec3 normal;
    normal.x = u.y*v.z-u.z*v.y;
    normal.y = u.z*v.x-u.x*v.z;
    normal.z = u.x*v.y-u.y*v.x;
    return glm::normalize(normal);
}