#version 330 core

layout (location = 0) in vec2 in_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_pos, 0.0f, 1.0f);
}