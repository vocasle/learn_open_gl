//
// Created by vocasle on 11/24/21.
//

#include <iostream>
#include <csignal>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "utility.h"

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

    Shader lighting_shader("lighting.shader");
    Shader color_shader("color.shader");

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

    unsigned int object_vao = 0;
    GL_CALL(glGenVertexArrays(1, &object_vao));
    GL_CALL(glBindVertexArray(object_vao));

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

    unsigned int light_vao = 0;
    GL_CALL(glGenVertexArrays(1, &light_vao));
    GL_CALL(glBindVertexArray(light_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));
    GL_CALL(glEnableVertexAttribArray(0));

    GL_CALL(glEnable(GL_DEPTH_TEST));

    glm::mat4 light_model(1.0f);
    light_model = glm::translate(light_model, glm::vec3(1.2f, 1.0f, 2.0f));
    light_model = glm::scale(light_model, glm::vec3(0.2f));
    const glm::mat4 object_model(1.0f);
    const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    color_shader.use();
    color_shader.set_mat4("model", object_model);
    color_shader.set_mat4("view", view);
    color_shader.set_mat4("projection", projection);

    lighting_shader.use();
    lighting_shader.set_mat4("model", light_model);
    lighting_shader.set_mat4("view", view);
    lighting_shader.set_mat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

//        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//        GL_CALL(glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)));

        lighting_shader.use();
        GL_CALL(glBindVertexArray(light_vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr));

        color_shader.use();
        GL_CALL(glBindVertexArray(object_vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);

    glfwTerminate();
}