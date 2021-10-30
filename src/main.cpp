#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    static bool fill = true;
    static bool e_pressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        e_pressed = true;
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && e_pressed) {
        fill = !fill;
        glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
        e_pressed = false;
    }
}

void check_shader_compile_error(uint shader)
{
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}

void check_program_link_error(uint program)
{
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    constexpr const char *vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertex_color;
void main()
{
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  vertex_color = vec4(0.5f, 0.0f, 0.0f, 1.0f);
}
)";

    uint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    check_shader_compile_error(vertex_shader);

    constexpr const char *fragment_shader_source = R"(
#version 330 core
out vec4 frag_color;
in vec4 vertex_color;

void main()
{
    frag_color = vertex_color;
}
)";

    constexpr const char *yellow_fragment_shader_source = R"(
#version 330 core
out vec4 frag_color;

void main()
{
    frag_color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
)";

    uint fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    check_shader_compile_error(fragment_shader);

    uint yellow_fragment_shader;
    yellow_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(yellow_fragment_shader, 1, &yellow_fragment_shader_source, nullptr);
    glCompileShader(yellow_fragment_shader);
    check_shader_compile_error(yellow_fragment_shader);

    uint shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    check_program_link_error(shader_program);
//    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    uint yellow_shader_program;
    yellow_shader_program = glCreateProgram();
    glAttachShader(yellow_shader_program, vertex_shader);
    glAttachShader(yellow_shader_program, yellow_fragment_shader);
    glLinkProgram(yellow_shader_program);
    check_program_link_error(yellow_shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(yellow_fragment_shader);

    constexpr float vert1[] = {
            -0.5f, -0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f
    };

    constexpr float vert2[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f
    };

    constexpr const uint indices[] = {
            0, 1, 2,
    };

    uint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert1), vert1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    uint VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    uint EBO2;
    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    uint VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        glUseProgram(yellow_shader_program);
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
