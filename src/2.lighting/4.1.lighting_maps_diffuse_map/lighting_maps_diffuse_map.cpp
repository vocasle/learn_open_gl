//
// Created by vocasle on 11/24/21.
//

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "utility.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"
#include "texture.h"

void process_input(GLFWwindow *window, Camera &camera, double delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
        camera.update_pos(Direction::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
        camera.update_pos(Direction::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
        camera.update_pos(Direction::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
        camera.update_pos(Direction::RIGHT, delta_time);
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

int main()
{
    constexpr int win_width = 800;
    constexpr int win_height = 600;
    GLFWwindow* window = init_gl_context(win_width, win_height);
    if (!window) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    const glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    Camera camera(1.0f, camera_pos);
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

    Shader lighting_shader("4.1.lighting.vert", "4.1.lighting.frag");
    Shader object_shader("4.1.object.vert", "4.1.object.frag");

    const std::vector<float> vertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    VertexArray object_va;
    VertexBuffer vb(vertices.data(), sizeof(float) * vertices.size());
    VertexBufferLayout vbl;
    vbl.add_element<float>(3);
    vbl.add_element<float>(3);
    vbl.add_element<float>(2);
    object_va.add_buffer(vb, vbl);

    VertexArray light_va;
    light_va.add_buffer(vb, vbl);

    GL_CALL(glEnable(GL_DEPTH_TEST));

    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), win_width/static_cast<float>(win_height), 0.1f,
                                                  100.0f);
    glm::vec3 light_pos(0.5f, 1.2f, 2.0f);
    glm::mat4 light_model(1.0f);
    light_model = glm::translate(light_model, light_pos);
    light_model = glm::scale(light_model, glm::vec3(0.2f));
    const glm::mat3 normal_matrix = glm::transpose(glm::inverse(light_model));

    double begin = glfwGetTime();
    double end = 0.0;
    double time_span = 0.0;
    float angle = 1.0f;

    Light light{ light_pos, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f) };
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
    Texture diffuse_map("assets/container2.png");
    Texture specular_map("assets/container2_specular.png");

    while (!glfwWindowShouldClose(window)) {
        end = glfwGetTime();
        time_span = end-begin;
        begin = end;
        process_input(window, camera, time_span);
        angle += static_cast<float>(time_span);

        GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        //light_pos = glm::vec3(
                //cos(angle) * 2.0f,
                //0.0f,
                //sin(angle) * 2.0f);
        //light_model = glm::translate(glm::mat4(1.0f), light_pos);
        //light_model = glm::scale(light_model, glm::vec3(0.2f));

        //const glm::vec3 light_color(sin(glfwGetTime() * 2.0f),
        //    sin(glfwGetTime() * 0.7f),
        //    sin(glfwGetTime() * 1.3f));
        //const glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
        //const glm::vec3 ambient_color = light_color * glm::vec3(0.2f);
        //light.diffuse = diffuse_color;
        //light.ambient = ambient_color;

        object_shader.use();
        object_shader.set_mat4("model", glm::mat4(1.0f));
        object_shader.set_mat4("view", camera.get_view());
        object_shader.set_mat4("projection", projection);
        object_shader.set_mat3("u_normal_mat", normal_matrix);
        object_shader.set_vec3("u_camera_pos", camera_pos);
        //object_shader.set_vec3("u_material.ambient", m.ambient);
        object_shader.set_int("u_material.diffuse", 0);
        object_shader.set_int("u_material.specular", 1);
        object_shader.set_float("u_material.shininess", m.shininess);
        object_shader.set_vec3("u_light.position", light_pos);
        object_shader.set_vec3("u_light.ambient", light.ambient);
        object_shader.set_vec3("u_light.diffuse", light.diffuse);
        object_shader.set_vec3("u_light.specular", light.specular);
        object_va.bind();
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        diffuse_map.bind();
        GL_CALL(glActiveTexture(GL_TEXTURE1));
        specular_map.bind();
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));

        lighting_shader.use();
        lighting_shader.set_mat4("model", light_model);
        lighting_shader.set_mat4("view", camera.get_view());
        lighting_shader.set_mat4("projection", projection);
        lighting_shader.set_vec3("u_light.position", light_pos);
        lighting_shader.set_vec3("u_light.ambient", light.ambient);
        lighting_shader.set_vec3("u_light.diffuse", light.diffuse);
        lighting_shader.set_vec3("u_light.specular", light.specular);

        light_va.bind();
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}