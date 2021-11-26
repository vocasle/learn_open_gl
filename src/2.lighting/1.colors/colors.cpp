//
// Created by vocasle on 11/24/21.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <csignal>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderSource {
    std::string vertex;
    std::string fragment;
};

static void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool gl_log_call(const std::string &function_name, const std::string &filename, unsigned int line)
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

#define ASSERT(x) if (!(x)) raise(SIGTRAP);

#define GL_CALL(x) gl_clear_error(); \
    x;                               \
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

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
    glValidateProgram(program);
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
        return 0;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

static unsigned int create_program(const ShaderSource &ss)
{
    return compile_program(compile_shader(GL_VERTEX_SHADER, ss.vertex),
                           compile_shader(GL_FRAGMENT_SHADER, ss.fragment));
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow *init_gl_context(int width, int height)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

int main()
{
    GLFWwindow *window = init_gl_context(800, 600);
    if (!window) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    unsigned int lighting_program = create_program(load_shader_source("lighting.shader"));
    unsigned int color_program = create_program(load_shader_source("color.shader"));

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(1.0f, -1.0f, -5.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    GL_CALL(glUseProgram(color_program));
    GL_CALL(unsigned int model_loc = glGetUniformLocation(color_program, "model"));
    GL_CALL(unsigned int view_loc = glGetUniformLocation(color_program, "view"));
    GL_CALL(unsigned int projection_loc = glGetUniformLocation(color_program, "projection"));

    GL_CALL(glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection)));

    constexpr float vertices[] = {
        -0.5f, -0.5f, 0.5f, // 0
        -0.5f, 0.5f, 0.5f,  // 1
        0.5f, 0.5f, 0.5f,   // 2
        0.5f, -0.5f, 0.5f,  // 3
        -0.5f, 0.5f, -0.5f, // 4
        0.5f, 0.5f, -0.5f,  // 5
        0.5f, -0.5f, -0.5f, // 6
        -0.5f, -0.5f, -0.5f,// 7
    };

    constexpr unsigned int indices[] = {
        // front
        0, 1, 2,
        0, 2, 3,
        // right
        3, 2, 5,
        3, 5, 6,
        // back
        6, 5, 4,
        6, 4, 7,
        // left
        7, 4, 1,
        7, 1, 0,
        // top
        1, 4, 5,
        1, 5, 2,
        // bottom
        3, 6, 0,
        0, 6, 7
    };

    unsigned int vao = 0;
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    unsigned int vbo = 0;
    GL_CALL(glGenBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    unsigned int ebo = 0;
    GL_CALL(glGenBuffers(1, &ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));
    GL_CALL(glEnableVertexAttribArray(0));

    double t_start = glfwGetTime();
    double t_end = 0.0;

    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        t_end = glfwGetTime();

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        GL_CALL(glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)));

        GL_CALL(glUseProgram(color_program));
        GL_CALL(glBindVertexArray(vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr));

//        shader.use();
//
//        glPolygonMode(GL_FRONT_AND_BACK, input_data.toggle_fill_mode ? GL_FILL : GL_LINE);
//        shader.set_float("in_ratio", input_data.ratio);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, texture2);
//
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);

    glfwTerminate();
}