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
#include "input_manager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
        camera.update_pos(Direction::FORWARD, 0);
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
        camera.update_pos(Direction::BACKWARD, 0);
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
        camera.update_pos(Direction::LEFT, 0);
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
        camera.update_pos(Direction::RIGHT, 0);
}

GLFWwindow* init_gl_context(int width, int height)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
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
    GLFWwindow* window = init_gl_context(800, 600);
    if (!window) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    Camera camera(1.0f, glm::vec3(0.0f, 0.0f, 10.0f));
    InputManager input_manager;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        static bool first_time = false;
        static double last_x = 0.0;
        static double last_y = 0.0;
        if (auto camera = static_cast<Camera*>(glfwGetWindowUserPointer(window))) {
            if (first_time) {
                last_x = x;
                last_y = y;
                first_time = false;
            }
            camera->update_euler_angles(x-last_x, last_y-y);
            last_x = x;
            last_y = y;
        }
    });

    Shader lighting_shader("lighting.shader");
    Shader object_shader("object.shader");

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

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr));
    GL_CALL(glEnableVertexAttribArray(0));

    unsigned int light_vao = 0;
    GL_CALL(glGenVertexArrays(1, &light_vao));
    GL_CALL(glBindVertexArray(light_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr));
    GL_CALL(glEnableVertexAttribArray(0));

    GL_CALL(glEnable(GL_DEPTH_TEST));

    glm::mat4 light_model(1.0f);
    light_model = glm::translate(light_model, glm::vec3(1.2f, 1.0f, 2.0f));
    light_model = glm::scale(light_model, glm::vec3(0.2f));
    const glm::mat4 object_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    input_manager.register_listener({GLFW_KEY_W, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::FORWARD, delta_time);
    }});
    input_manager.register_listener({GLFW_KEY_S, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::BACKWARD, delta_time);
    }});
    input_manager.register_listener({GLFW_KEY_A, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::LEFT, delta_time);
    }});
    input_manager.register_listener({GLFW_KEY_D, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::RIGHT, delta_time);
    }});
    input_manager.register_listener({GLFW_KEY_ESCAPE, KeyState::PRESSED, [window](double delta_time) {
        glfwSetWindowShouldClose(window, true);
    }});

    object_shader.use();
    object_shader.set_mat4("model", object_model);
    object_shader.set_mat4("projection", projection);
    object_shader.set_vec4("u_object_color", glm::vec4(1.0f, 0.5f, 0.31f, 1.0f));
    object_shader.set_vec4("u_light_color", glm::vec4(1.0f));

    lighting_shader.use();
    lighting_shader.set_mat4("model", light_model);
    lighting_shader.set_mat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        input_manager.process_keyboard_input(window);

        GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        lighting_shader.use();
        lighting_shader.set_mat4("view", camera.get_view());
        GL_CALL(glBindVertexArray(light_vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, nullptr));

        object_shader.use();
        object_shader.set_mat4("view", camera.get_view());
        GL_CALL(glBindVertexArray(object_vao));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    GL_CALL(glDeleteVertexArrays(1, &object_vao));
    GL_CALL(glDeleteVertexArrays(1, &light_vao));
    GL_CALL(glDeleteBuffers(1, &vbo));

    glfwTerminate();
}