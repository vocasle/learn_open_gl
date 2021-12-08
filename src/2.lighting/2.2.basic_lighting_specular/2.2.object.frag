#version 330 core

in vec3 extern_color;

out vec4 out_color;

void main()
{
    out_color = vec4(extern_color, 1.0);
}