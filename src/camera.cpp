//
// Created by vocasle on 11/27/21.
//

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "camera.h"

Camera::Camera(float in_camera_speed, const glm::vec3& in_camera_pos)
        :
        camera_pos(in_camera_pos),
        camera_front(0.0f, 0.0f, -1.0f),
        camera_up(),
        camera_right(),
        camera_speed(in_camera_speed),
        pitch(0.0f),
        yaw(-90.0f),
        mouse_sensitivity(0.05)
{
}

#include <iostream>

glm::mat4 Camera::get_view() const
{
    return glm::lookAt(camera_pos, camera_pos+camera_front, camera_up);
}

void Camera::update_vectors()
{
    glm::vec3 direction;
    const float cos_pitch = std::cos(glm::radians(pitch));
    direction.x = std::cos(glm::radians(yaw))*cos_pitch;
    direction.y = std::sin(glm::radians(pitch));
    direction.z = std::sin(glm::radians(yaw))*cos_pitch;
    camera_front = glm::normalize(direction);
    camera_right = glm::normalize(glm::cross(camera_front, world_up));
    camera_up = glm::normalize(glm::cross(camera_right, camera_front));
}

void Camera::update_pos(Direction d, double delta_time)
{
    float speed = static_cast<float>(delta_time)*camera_speed;
    switch (d) {
    case Direction::FORWARD:
        camera_pos += speed*camera_front;
        break;
    case Direction::BACKWARD:
        camera_pos -= speed*camera_front;
        break;
    case Direction::LEFT:
        camera_pos -= camera_right*speed;
        break;
    case Direction::RIGHT:
        camera_pos += camera_right*speed;
        break;
    }
}

void Camera::update_euler_angles(double x, double y)
{
    x *= mouse_sensitivity;
    y *= mouse_sensitivity;
    yaw += x;
    pitch += y;

    if (pitch>max_pitch)
        pitch = max_pitch;
    if (pitch<-max_pitch)
        pitch = -max_pitch;

    update_vectors();
}
