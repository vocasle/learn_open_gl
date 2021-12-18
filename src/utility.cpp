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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    const auto container = static_cast<GlfwContainer*>(glfwGetWindowUserPointer(window));
    container->win_height = height;
    container->win_width = width;
    glViewport(0, 0, width, height);
}

GLFWwindow* init_gl_context(int width, int height)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

std::string format(const std::string& fmt)
{
    return fmt;
}

void APIENTRY gl_debug_message_callback(int source,
                               int type,
                               unsigned int severity,
                               int length,
                               const char *message,
                               const void *user_param)
{
    std::cerr << "OPENGL::ERROR::" << message << std::endl;
}

void gl_print_debug_info() {
    std::cout << "OpenGL version: "
        << glGetString(GL_VERSION)
        << "\nOpenGL Vendor: "
        << glGetString(GL_VENDOR)
        << std::endl;
}

void gl_enable_debug_output() {
    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
}
