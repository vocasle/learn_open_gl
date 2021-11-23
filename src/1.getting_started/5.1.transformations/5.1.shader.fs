#version 330 core

in vec2 texture_coord;

out vec4 out_color;
uniform sampler2D out_texture;

uniform vec4 in_color;

void main()
{
    out_color = in_color * texture(out_texture, texture_coord);
    if (out_color.rgb == vec3(0)) discard;
}