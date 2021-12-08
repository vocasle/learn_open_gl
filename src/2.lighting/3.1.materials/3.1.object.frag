#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}

in vec3 extern_normal;
in vec3 extern_frag_pos;

uniform Material u_material;
uniform vec3 u_object_color;
uniform vec3 u_light_color;
uniform vec3 u_light_pos;
uniform mat3 u_normal_mat;
uniform vec3 u_camera_pos;

out vec4 out_color;

void main()
{
    // ambient
    vec3 ambient_component =  u_light_color * u_material.ambient;

   // diffuse
   vec3 normal = normalize(u_normal_mat * extern_normal);
   vec3 light_direction = normalize(u_light_pos - extern_frag_pos);
   float diffuse = max(dot(normal, light_direction), 0.0);
   vec3 diffuse_component = u_light_color * (diffuse * u_material.diffuse);

   // specular
   vec3 camera_direction = normalize(-u_camera_pos + extern_frag_pos);
   vec3 reflected_ray = reflect(light_direction, normal);
   float specular = pow(max(dot(camera_direction, reflected_ray), 0.0), u_material.shininess);
   vec3 specular_component =  u_light_color * (specular * u_material.specular);

    vec3 result_color = (ambient_component + diffuse_component + specular_component) * u_object_color;
    out_color = vec4(result_color, 1.0);
}