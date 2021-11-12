#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 transform;

out vec3 color;

void main()
{
    vec4 pos = transform * vec4(in_position, 1.0);

    gl_Position = pos;
}