//
// Created by vocasle on 11/27/21.
//

#ifndef LEARN_OPEN_GL_INCLUDE_UTILITY_H
#define LEARN_OPEN_GL_INCLUDE_UTILITY_H

#include <csignal>

void gl_clear_error();
bool gl_log_call(const std::string &function_name, const std::string &filename, unsigned int line);

#define ASSERT(x) if (!(x)) raise(SIGTRAP);

#define GL_CALL(x) gl_clear_error(); \
    x;                               \
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

#endif //LEARN_OPEN_GL_INCLUDE_UTILITY_H
