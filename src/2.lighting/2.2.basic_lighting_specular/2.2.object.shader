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
uniform mat3 u_normal_mat;
uniform vec3 u_camera_pos;

out vec4 out_color;

void main()
{
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient_component = ambient_strength * u_light_color;

   // diffuse
   vec3 normal = normalize(u_normal_mat * extern_normal);
   vec3 light_direction = normalize(u_light_pos - extern_frag_pos);
   float diffuse = max(dot(normal, light_direction), 0.0);
   vec3 diffuse_component = diffuse * u_light_color;

   // specular
   float specular_strength = 0.5;
   vec3 camera_direction = normalize(-u_camera_pos + extern_frag_pos);
   vec3 reflected_ray = reflect(light_direction, normal);
   float specular = pow(max(dot(camera_direction, reflected_ray), 0.0), 32);
   vec3 specular_component = specular_strength * specular * u_light_color;

    vec3 result_color = (ambient_component + diffuse_component + specular_component) * u_object_color;
    out_color = vec4(result_color, 1.0);
}