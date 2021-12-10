#version 330 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_light;

out vec4 out_color;

void main()
{
    out_color = vec4(u_light.ambient + u_light.diffuse + u_light.specular, 1.0);
}