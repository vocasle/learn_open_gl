#shader vertex
#version 330 core

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 extern_normal;
out vec3 extern_frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    extern_frag_pos = vec3(model * in_position);
    gl_Position = projection * view * model * in_position;
    extern_normal = in_normal;
}

#shader fragment
#version 330 core

in vec3 extern_normal;
in vec3 extern_frag_pos;

uniform vec3 u_object_color;
uniform vec3 u_light_color;
uniform vec3 u_light_pos;

out vec4 out_color;

void main()
{
    vec3 norm = normalize(extern_normal);
    vec3 light_dir = normalize(u_light_pos - extern_frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * u_light_color;
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * u_light_color;
    vec3 resulting_color = (ambient + diffuse) * u_object_color;
    out_color = vec4(resulting_color, 1.0f);
}