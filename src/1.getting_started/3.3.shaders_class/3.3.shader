#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
out vec3 vertex_pos;

void main()
{
    gl_Position = vec4(pos, 1.0);
    vertex_pos = pos;
}

#shader fragment
#version 330 core

out vec4 color;
in vec3 vertex_position;

void main()
{
    color = vec4(vertex_position, 1.0);
}