#version 330 core

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_text_coords;

out vec3 extern_normal;
out vec3 extern_frag_pos;
out vec2 extern_text_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    extern_frag_pos = vec3(model * in_position);
    extern_text_coords = in_text_coords;
    gl_Position = projection * view * model * in_position;
    //extern_normal = in_normal;
    extern_normal = mat3(transpose(inverse(model))) * in_normal; 
}