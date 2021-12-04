//
// Created by vocasle on 11/27/21.
//

#ifndef LEARN_OPEN_GL_INCLUDE_UTILITY_H
#define LEARN_OPEN_GL_INCLUDE_UTILITY_H

#include <glm/vec3.hpp>
void gl_clear_error();

bool gl_log_call(const std::string& function_name, const std::string& filename, unsigned int line);

#ifdef _WIN32
#define ASSERT(x) if (!(x)) __debugbreak();
#else
#include <csignal>
#define ASSERT(x) if (!(x)) raise(SIGTRAP);
#endif

#define GL_CALL(x) gl_clear_error(); \
    x;                               \
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

glm::vec3 calc_normal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

#endif //LEARN_OPEN_GL_INCLUDE_UTILITY_H
