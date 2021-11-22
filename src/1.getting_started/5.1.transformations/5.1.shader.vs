#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_text_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float in_rotation;

out vec3 color;
out vec2 texture_coord;

void main()
{
    gl_Position = projection * view * model * vec4(in_position, 1.0);
    float mid = 0.5;
    texture_coord = vec2(
        cos(in_rotation) * (in_text_coord.x - mid) + sin(in_rotation) * (in_text_coord.y - mid) + mid,
        cos(in_rotation) * (in_text_coord.y - mid) - sin(in_rotation) * (in_text_coord.x - mid) + mid
        );
}