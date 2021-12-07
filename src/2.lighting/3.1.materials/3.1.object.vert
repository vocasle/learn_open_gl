#version 330 core

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_normal;

out vec4 extern_frag_pos;
out vec4 extern_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    extern_frag_pos = model * in_position;
    extern_normal = transpose(inverse(model)) * in_normal;

    gl_Position = projection * view * model * in_position;
}