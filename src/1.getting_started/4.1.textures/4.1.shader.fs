#version 330 core

in vec2 texture_coord;
in vec3 color;

uniform sampler2D out_texture;
uniform sampler2D out_texture2;

out vec4 out_color;

void main()
{
    vec2 reversed_face_coord = vec2(-texture_coord.x, texture_coord.y);
    out_color = mix(texture(out_texture, texture_coord), texture(out_texture2, reversed_face_coord), 0.5);
}