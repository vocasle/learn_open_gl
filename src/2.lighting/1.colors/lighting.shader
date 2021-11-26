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

out vec4 out_color;

uniform vec4 object_color;
uniform vec4 light_color;

void main()
{
    out_color = light_color * object_color;
}