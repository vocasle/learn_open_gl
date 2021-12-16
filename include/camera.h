//
// Created by vocasle on 11/27/21.
//

#ifndef LEARN_OPEN_GL_SRC_CAMERA_H
#define LEARN_OPEN_GL_SRC_CAMERA_H

#include <glm/glm.hpp>

enum class Direction {
    FORWARD, BACKWARD, LEFT, RIGHT
};

class Camera {
public:
    Camera(float in_camera_speed, const glm::vec3& in_camera_pos);

    [[nodiscard]] glm::mat4 get_view() const;

    void update_pos(Direction d, double delta_time);
    void update_euler_angles(double x, double y);
    void toggle_acceleration(bool enable_accel);
    glm::vec3 get_front() const;
    glm::vec3 get_position() const;

private:
    void update_vectors();

    glm::vec3 camera_pos;
    glm::vec3 camera_front;
    glm::vec3 camera_up;
    glm::vec3 camera_right;
    float camera_speed;
    double pitch;
    double yaw;
    double mouse_sensitivity;

    static constexpr glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr float max_pitch = 89.0f;
    static constexpr float default_speed = 1.0f;
};

#endif //LEARN_OPEN_GL_SRC_CAMERA_H
