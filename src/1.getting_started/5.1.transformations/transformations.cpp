//
// Created by vocasle on 11/7/21.
//

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

struct VertexData {
    uint VAO = 0;
    uint VBO = 0;
    uint EBO = 0;
    std::vector<float> vertices;
    std::vector<uint> indices;
};

enum class Direction {
    up, down, left, right, none
};

struct Controller {
    Direction direction = Direction::none;
    glm::mat4 trans = glm::mat4(1.0f);
    float speed = 0.01f;
};

void update_controls(Controller& controller)
{
    float speed = controller.speed * 1.0f;
    switch (controller.direction) {
    case Direction::up:
        controller.trans = glm::translate(controller.trans, glm::vec3(0.0f, speed , 0.0f));
        break;
    case Direction::down:
        controller.trans = glm::translate(controller.trans, glm::vec3(0.0f, -speed, 0.0f));
        break;
    case Direction::left:
        controller.trans = glm::translate(controller.trans, glm::vec3(-speed, 0.0f, 0.0f));
        break;
    case Direction::right:
        controller.trans = glm::translate(controller.trans, glm::vec3(speed, 0.0f, 0.0f));
        break;
    case Direction::none:
        break;
    }

    if (controller.trans[3][0] > 1.0f)
        controller.trans[3][0] = -1.0f;
    else if (controller.trans[3][0] < -1.0f)
        controller.trans[3][0] = 1.0f;

    if (controller.trans[3][1] > 1.0f)
        controller.trans[3][1] = -1.0f;
    else if (controller.trans[3][1] < -1.0f)
        controller.trans[3][1] = 1.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Controller& controller)
{
    static bool fill = true;
    static bool e_pressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_E)==GLFW_PRESS)
        e_pressed = true;
    else if (glfwGetKey(window, GLFW_KEY_E)==GLFW_RELEASE && e_pressed) {
        fill = !fill;
        glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
        e_pressed = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        controller.direction = Direction::up;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        controller.direction = Direction::down;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        controller.direction = Direction::left;
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        controller.direction = Direction::right;
}

GLFWwindow* init_glfw();

VertexData init_vertices()
{
    VertexData vertex_data;
    vertex_data.vertices = {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.1f, 0.0f,
            0.1f, 0.0f, 0.0f,
            0.1f, 0.1f, 0.0f
    };

    vertex_data.indices = {
            0, 1, 2,
            1, 2, 3
    };

    glGenVertexArrays(1, &vertex_data.VAO);
    glBindVertexArray(vertex_data.VAO);

    glGenBuffers(1, &vertex_data.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.vertices.size() * sizeof(float), &vertex_data.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertex_data.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_data.indices.size() * sizeof(uint), &vertex_data.indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(NULL));
    glEnableVertexAttribArray(0);

    return vertex_data;
}

int main()
{
    GLFWwindow* window = init_glfw();
    if (!window)
        return -1;

    Shader shader("5.1.shader.vs", "5.1.shader.fs");
    VertexData vertex_data = init_vertices();
    Controller controller;

    while (!glfwWindowShouldClose(window)) {
        processInput(window, controller);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        update_controls(controller);
        uint transform_loc = glGetUniformLocation(shader.program_id, "transform");
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(controller.trans));

        glBindVertexArray(vertex_data.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

GLFWwindow* init_glfw()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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
