//
// Created by vocasle on 11/24/21.
//

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "camera.h"
#include "shader.h"
#include "utility.h"
#include "input_manager.h"
#include "vertex_buffer.h"
#include "vertex_array.h"
#include "vertex_buffer_layout.h"

int main()
{
    constexpr int win_width = 800;
    constexpr int win_height = 600;
    GLFWwindow* window = init_gl_context(win_width, win_height);
    if (!window) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    constexpr glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    Camera camera(1.0f, camera_pos);
    InputManager input_manager;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        static bool first_time = false;
        static double last_x = 0.0;
        static double last_y = 0.0;
        if (const auto c = static_cast<GlfwContainer*>(glfwGetWindowUserPointer(w))) {
            if (first_time) {
                last_x = x;
                last_y = y;
                first_time = false;
            }
            c->camera.update_euler_angles(x - last_x, last_y - y);
            last_x = x;
            last_y = y;
        }
        });

    Shader lighting_shader("2.2.lighting.vert", "2.2.lighting.frag");
    Shader object_shader("2.2.object.vert", "2.2.object.frag");

    const std::vector<float> vertices = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };
    VertexBuffer vb(vertices.data(), sizeof(float) * vertices.size());
    VertexArray object_va;
    VertexBufferLayout vbl;
    vbl.add_element<float>(3);
    vbl.add_element<float>(3);
    object_va.add_buffer(vb, vbl);

    VertexArray light_va;
    VertexBufferLayout light_vbl;
    light_va.add_buffer(vb, vbl);

    input_manager.register_listener({ GLFW_KEY_W, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::FORWARD, delta_time);
    } });
    input_manager.register_listener({ GLFW_KEY_S, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::BACKWARD, delta_time);
    } });
    input_manager.register_listener({ GLFW_KEY_A, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::LEFT, delta_time);
    } });
    input_manager.register_listener({ GLFW_KEY_D, KeyState::PRESSED, [&camera](double delta_time) {
        camera.update_pos(Direction::RIGHT, delta_time);
    } });
    input_manager.register_listener({ GLFW_KEY_ESCAPE, KeyState::PRESSED, [window](double delta_time) {
        glfwSetWindowShouldClose(window, true);
    } });

    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), win_width / static_cast<float>(win_height), 0.1f,
        100.0f);
    glm::vec3 light_pos(0.0f, 0.0f, 0.0f);
    glm::mat4 light_model(1.0f);
    const glm::mat3 normal_matrix = glm::transpose(glm::inverse(light_model));

    double begin = glfwGetTime();
    double end = 0.0;
    double time_span = 0.0;

    GL_CALL(glEnable(GL_DEPTH_TEST));

    while (!glfwWindowShouldClose(window)) {
        input_manager.process_keyboard_input(window);

        GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        end = glfwGetTime();
        time_span = end - begin;

        light_pos = glm::vec3(
            cos(time_span) * 2.0,
            0.0f,
            sin(time_span) * 2.0);
        light_model = glm::translate(glm::mat4(1.0f), light_pos);
        light_model = glm::scale(light_model, glm::vec3(0.2f));

        object_shader.use();
        object_shader.set_mat4("model", glm::mat4(1.0f));
        object_shader.set_mat4("view", camera.get_view());
        object_shader.set_mat4("projection", projection);
        object_shader.set_vec3("u_object_color", glm::vec3(1.0f, 0.5f, 0.31f));
        object_shader.set_vec3("u_light_color", glm::vec3(1.0f));
        object_shader.set_vec3("u_light_pos", light_pos);
        object_shader.set_mat3("u_normal_mat", normal_matrix);
        object_shader.set_vec3("u_camera_pos", camera_pos);
        object_va.bind();
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));

        lighting_shader.use();
        lighting_shader.set_mat4("model", light_model);
        lighting_shader.set_mat4("view", camera.get_view());
        lighting_shader.set_mat4("projection", projection);
        light_va.bind();
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }    

    //glfwTerminate();
}