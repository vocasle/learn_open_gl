#version 330 core

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 extern_normal;
out vec3 extern_frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    extern_frag_pos = vec3(model * in_position);
    gl_Position = projection * view * model * in_position;
    extern_normal = in_normal;
}