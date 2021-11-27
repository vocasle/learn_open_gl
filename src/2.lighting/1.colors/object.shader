#shader vertex
#version 330 core

layout (location = 0) in vec4 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * in_position;
}

#shader fragment
#version 330 core

uniform vec4 u_object_color;
uniform vec4 u_light_color;

out vec4 out_color;

void main()
{
    out_color = u_light_color * u_object_color;
}