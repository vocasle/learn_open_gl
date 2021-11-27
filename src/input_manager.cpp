//
// Created by vocasle on 11/27/21.
//

#include "input_manager.h"

void InputManager::process_keyboard_input(GLFWwindow* window)
{
    end_time = glfwGetTime();
    delta_time = end_time - start_time;
    start_time = end_time;
    for (auto& kvp: event_handlers) {
        KeyState state = glfwGetKey(window, kvp.first)==GLFW_PRESS ? KeyState::PRESSED : KeyState::RELEASED;
        if (state==kvp.second.key_state || kvp.second.key_state==KeyState::UNSPECIFIED)
            kvp.second.handler(delta_time);
    }
}

void InputManager::process_mouse_input(GLFWwindow* window, double x, double y)
{

}

void InputManager::register_listener(const KeyEventHandler& event_handler)
{
    event_handlers[event_handler.key] = event_handler;
}

InputManager::InputManager():
    event_handlers(),
    last_mouse_x(0.0f),
    last_mouse_y(0.0f),
    delta_time(0.0),
    start_time(0.0),
    end_time(0.0)
{

}
