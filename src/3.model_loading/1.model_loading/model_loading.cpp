#include <iostream>



//
// Created by vocasle on 11/24/21.
//

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "utility.h"
#include "model.h"

void process_input(GLFWwindow *window, Camera &camera, double delta_time) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.update_pos(Direction::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.update_pos(Direction::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.update_pos(Direction::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.update_pos(Direction::RIGHT, delta_time);

    camera.toggle_acceleration(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
}

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

int main() {
    int win_width = 800;
    int win_height = 600;

    GLFWwindow *window = init_gl_context(win_width, win_height);
    if (!window) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);
    gl_print_debug_info();
    gl_enable_debug_output();
    GL_CALL(glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(gl_debug_message_callback), nullptr));

    const glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    Camera camera(1.0f, camera_pos);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    GlfwContainer container {camera, win_width, win_height};
    glfwSetWindowUserPointer(window, &container);
    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        static bool first_time = false;
        static double last_x = 0.0;
        static double last_y = 0.0;
        if (const GlfwContainer *c = static_cast<GlfwContainer *>(glfwGetWindowUserPointer(w))) {
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

    Shader object_shader("shader.vert", "shader.frag");

    constexpr unsigned int NUM_PONT_LIGHTS = 4;
    constexpr glm::vec3 point_lights_positions[NUM_PONT_LIGHTS] = {
            {0.7f,  0.2f,  2.0f},
            {2.3f,  -3.3f, -4.0f},
            {-4.0f, 2.0f,  -12.0f},
            {0.0f,  0.0f,  -3.0f}
    };

    GL_CALL(glEnable(GL_DEPTH_TEST));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), win_width / static_cast<float>(win_height), 0.1f,
                                                  100.0f);

    double begin = glfwGetTime();
    double end = 0.0;
    double time_span = 0.0;
    float angle = 1.0f;

    Light light{
            {1.2f, 1.0f, 2.0f},
            glm::vec3(0.1f, 0.0f, 0.0f),
            glm::vec3(0.1f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)};
    Material bronze{
            glm::vec3(0.2125f, 0.1275f, 0.054f),
            glm::vec3(0.714f, 0.4284f, 0.18144f),
            glm::vec3(0.393548f, 0.271906f, 0.166721f),
            0.2f * 128.0f
    };

    Material chrome{
            glm::vec3(0.25f),
            glm::vec3(0.4f),
            glm::vec3(0.774597f),
            0.6f * 128.0f
    };

    Material m = chrome;
    Model backpack_model("../../assets/backpack.obj");

    while (!glfwWindowShouldClose(window)) {
        end = glfwGetTime();
        time_span = end - begin;
        begin = end;
        process_input(window, camera, time_span);
        angle += static_cast<float>(time_span);

        if (container.win_height != win_height || container.win_width != win_width) {
            win_height = container.win_height;
            win_width = container.win_width;
            projection = glm::perspective(glm::radians(45.0f), win_width / static_cast<float>(win_height), 0.1f,
                                          100.0f);
        }

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        object_shader.use();
        object_shader.set_mat4("view", camera.get_view());
        object_shader.set_mat4("projection", projection);
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0));
        model = glm::scale(model, glm::vec3(1.0f));
        object_shader.set_mat4("model", model);
        backpack_model.Draw(object_shader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
