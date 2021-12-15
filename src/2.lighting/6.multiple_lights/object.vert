#version 330 core

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_text_coords;

out vec3 normal;
out vec3 frag_pos;
out vec2 text_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    frag_pos = vec3(model * in_position);
    text_coords = in_text_coords;
    normal = in_normal;
    gl_Position = projection * view * model * in_position;
}
