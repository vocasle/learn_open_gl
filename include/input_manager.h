//
// Created by vocasle on 11/27/21.
//

#ifndef LEARN_OPEN_GL_INPUT_MANAGER_H
#define LEARN_OPEN_GL_INPUT_MANAGER_H

#include <functional>
#include <unordered_map>

#include <GLFW/glfw3.h>

enum class KeyState {
    PRESSED, RELEASED, UNSPECIFIED
};

struct KeyEventHandler {
    int key;
    KeyState key_state;
    std::function<void(double)> handler;
};

class InputManager {
public:
    InputManager();

    void process_mouse_input(GLFWwindow* window, double x, double y);

    void process_keyboard_input(GLFWwindow* window);

    void register_listener(const KeyEventHandler &event_handler);

private:
    std::unordered_map<int, KeyEventHandler> event_handlers;
    float last_mouse_x;
    float last_mouse_y;
    double delta_time;
    double start_time;
    double end_time;
};

#endif //LEARN_OPEN_GL_INPUT_MANAGER_H
