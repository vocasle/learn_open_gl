#version 330 core

in vec2 texture_coord;
in vec3 color;

uniform sampler2D out_texture;
uniform sampler2D out_texture2;

out vec4 out_color;

void main()
{
    out_color = mix(texture(out_texture, texture_coord), texture(out_texture2, texture_coord), 0.2);
}