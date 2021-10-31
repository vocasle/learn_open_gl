#version 330 core

layout (location = 0) in vec3 pos;
out vec3 vertex_pos;

void main()
{
    gl_Position = vec4(pos, 1.0);
    vertex_pos = pos;
}