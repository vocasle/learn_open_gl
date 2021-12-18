//
// Created by vocasle on 11/27/21.
//

#ifndef LEARN_OPEN_GL_INCLUDE_UTILITY_H
#define LEARN_OPEN_GL_INCLUDE_UTILITY_H

#include <string>
#include <glm/vec3.hpp>

#include "GLFW/glfw3.h"

class Camera;
struct GlfwContainer {
	Camera& camera;
	int win_width;
	int win_height;
};

void gl_clear_error();

bool gl_log_call(const std::string& function_name, const std::string& filename, unsigned int line);

#ifdef _WIN32
#define ASSERT(x) if (!(x)) __debugbreak()
#else
#include <csignal>
#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#endif

#define GL_CALL(x) gl_clear_error(); \
    x;                               \
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

glm::vec3 calc_normal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

void framebuffer_size_callback(GLFWwindow * window, int width, int height);

GLFWwindow* init_gl_context(int width, int height);

std::string format(const std::string & fmt);

template <typename Arg, typename ...Args>
std::string format(const std::string & fmt, Arg value, Args ...args)
{
	std::ostringstream out;
	for (auto it = std::begin(fmt); it != std::end(fmt); ++it) {
		if (*it == '{' && (it + 1) != std::end(fmt) && *(it + 1) == '}') {
			it += 2;
			out << value;
			out << format(std::string(it, std::end(fmt)), args...);
			break;
		}
		else
			out << *it;
	}
	return out.str();
}

void APIENTRY gl_debug_message_callback(GLenum source,
                               GLenum type,
                               GLuint severity,
                               GLsizei length,
                               const GLchar *message,
                               const void *user_param);

void gl_print_debug_info();
void gl_enable_debug_output();

#endif //LEARN_OPEN_GL_INCLUDE_UTILITY_H
