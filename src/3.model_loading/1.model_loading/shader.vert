#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_text_coords;

out vec2 text_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	text_coords = in_text_coords;
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
}
