//
// Created by vocasle on 11/27/21.
//
#include <string>
#include <iostream>

#include <glad/glad.h>

#include "utility.h"

void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

bool gl_log_call(const std::string &function_name, const std::string &filename, unsigned int line)
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