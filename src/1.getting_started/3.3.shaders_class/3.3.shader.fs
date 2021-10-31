#version 330 core

out vec4 color;
in vec3 vertex_position;

void main()
{
    color = vec4(vertex_position, 1.0);
}