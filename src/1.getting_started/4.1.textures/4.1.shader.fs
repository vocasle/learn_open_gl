#version 330 core

in vec2 texture_coord;
in vec3 color;

uniform sampler2D out_texture;

out vec4 out_color;

void main()
{
    out_color = texture(out_texture, texture_coord) * vec4(color, 1.0);
}