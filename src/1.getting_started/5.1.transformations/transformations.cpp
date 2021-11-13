//
// Created by vocasle on 11/7/21.
//

#include <iostream>
#include <vector>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <queue>

#include "miniaudio.h"

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

const char* direction_as_string(Direction direction)
{
    switch (direction) {
    case Direction::up:
        return "up";
        break;
    case Direction::down:
        return "down";
        break;
    case Direction::left:
        return "left";
        break;
    case Direction::right:
        return "right";
        break;
    case Direction::none:
        return "none";
        break;
    default:
        return "default";
    }
}

float direction_as_angle(Direction direction)
{
    switch (direction) {
    case Direction::up:
        return glm::radians(90.0f);
        break;
    case Direction::down:
        return glm::radians(270.0f);
        break;
    case Direction::left:
        return glm::radians(180.0f);
        break;
    case Direction::right:
        return glm::radians(0.0f);
        break;
    case Direction::none:
        return 0.0f;
        break;
    default:
        return 0.0f;
    }
}

struct Controller {
    static constexpr int cool_down_max = 10;
    Direction direction = Direction::none;
    Direction prev_direction = Direction::none;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    float speed = 0.01f;
    float angle = 0.0f;
    bool quit_game = false;
    std::queue<glm::mat4> positions;
    int cool_down = Controller::cool_down_max;
};

void clamp_translation(glm::mat4& trans)
{
    constexpr float max = 50.0f;
    if (trans[3][0]>max)
        trans[3][0] = -max;
    else if (trans[3][0]<-max)
        trans[3][0] = max;

    if (trans[3][1]>max)
        trans[3][1] = -max;
    else if (trans[3][1]<-max)
        trans[3][1] = max;
}

void update_controls(Controller& controller)
{
    float speed = 2.0f;
    const glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

    controller.view = glm::translate(controller.view, glm::vec3(speed, 0.0f, 0.0f));
    const bool is_moving_along_x_axis =
            (controller.direction==Direction::left || controller.direction==Direction::right)
                    && (controller.prev_direction==Direction::left || controller.prev_direction==Direction::right);
    if (controller.direction!=Direction::none && controller.direction!=controller.prev_direction
            && !is_moving_along_x_axis) {
        float angle = direction_as_angle(controller.direction);
        controller.view = glm::rotate(controller.view, angle-controller.angle, z_axis);
        controller.angle = angle;
    }

    controller.positions.push(controller.view);
    controller.prev_direction = controller.direction;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window, Controller& controller)
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

    else if (glfwGetKey(window, GLFW_KEY_UP)==GLFW_PRESS)
        controller.direction = Direction::up;
    else if (glfwGetKey(window, GLFW_KEY_DOWN)==GLFW_PRESS)
        controller.direction = Direction::down;
    else if (glfwGetKey(window, GLFW_KEY_LEFT)==GLFW_PRESS)
        controller.direction = Direction::left;
    else if (glfwGetKey(window, GLFW_KEY_RIGHT)==GLFW_PRESS)
        controller.direction = Direction::right;
    else if (glfwGetKey(window, GLFW_KEY_Q)==GLFW_PRESS)
        controller.quit_game = true;
}

GLFWwindow* init_glfw();

VertexData init_vertices()
{
    VertexData vertex_data;
    vertex_data.vertices = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    vertex_data.indices = {
            0, 1, 2,
            1, 2, 3
    };

    glGenVertexArrays(1, &vertex_data.VAO);
    glBindVertexArray(vertex_data.VAO);

    glGenBuffers(1, &vertex_data.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.vertices.size()*sizeof(float), &vertex_data.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertex_data.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_data.indices.size()*sizeof(uint), &vertex_data.indices[0],
            GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), reinterpret_cast<void*>(NULL));
    glEnableVertexAttribArray(0);

    return vertex_data;
}

void play_audio(const char* file_name);

int main()
{
    GLFWwindow* window = init_glfw();
    if (!window)
        return -1;

    Shader shader("5.1.shader.vs", "5.1.shader.fs");
    VertexData vertex_data = init_vertices();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    Controller controller;

    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    controller.view = view;
    controller.model = model;
    controller.projection = projection;

    glm::vec4 color(1.0f);
    glm::mat4 mat(1.0f);
    glm::mat4 mat2(1.0f);
    std::queue<glm::mat4> positions;
//    std::thread audio(play_audio, "../assets/Greenberg.mp3");

    while (!glfwWindowShouldClose(window)) {
        process_input(window, controller);
        --controller.cool_down;
        if (controller.cool_down<0) {
            update_controls(controller);
            controller.cool_down = Controller::cool_down_max;
        }

        if (controller.quit_game)
            break;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        GLint model_loc = glGetUniformLocation(shader.program_id, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(controller.model));
        GLint view_loc = glGetUniformLocation(shader.program_id, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(controller.view));
        GLint projection_loc = glGetUniformLocation(shader.program_id, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(controller.projection));
        GLint color_loc = glGetUniformLocation(shader.program_id, "in_color");
        color = glm::vec4(1.0f);
        glUniform4fv(color_loc, 1, glm::value_ptr(color));

        glBindVertexArray(vertex_data.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


//        glm::mat4 trans = glm::translate(controller.view, glm::vec3(-2.1f, 0.0f, 0.0f));
        if (controller.positions.size()>1) {
            mat = controller.positions.front();
            controller.positions.pop();
            positions.push(mat);
        }
        std::cout << "size: " << controller.positions.size() << std::endl;
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(controller.model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(mat));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(controller.projection));
        glUniform4fv(color_loc, 1, glm::value_ptr(color));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        if (positions.size()>1) {
            mat2 = positions.front();
            positions.pop();
        }
        color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(controller.model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(mat2));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(controller.projection));
        glUniform4fv(color_loc, 1, glm::value_ptr(color));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_bool32 isLooping = MA_TRUE;

    ma_decoder* pDecoder = (ma_decoder*) pDevice->pUserData;
    if (pDecoder==NULL) {
        return;
    }

    /*
    A decoder is a data source which means you can seemlessly plug it into the ma_data_source API. We can therefore take advantage
    of the "loop" parameter of ma_data_source_read_pcm_frames() to handle looping for us.
    */
    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL, isLooping);

    (void) pInput;
}

void play_audio(const char* file_name)
{
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    result = ma_decoder_init_file(file_name, NULL, &decoder);
    if (result!=MA_SUCCESS) {
        return;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device)!=MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    if (ma_device_start(&device)!=MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
}
