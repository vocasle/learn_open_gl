#version 330 core

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 extern_normal;
in vec3 extern_frag_pos;
in vec2 extern_text_coords;

uniform Material u_material;
uniform Light u_light;
uniform mat3 u_normal_mat;
uniform vec3 u_camera_pos;

out vec4 out_color;

void main()
{

    vec3 diffuse_map_color = vec3(texture(u_material.diffuse, extern_text_coords));
    // ambient
    vec3 ambient_component =  u_light.ambient * diffuse_map_color;

   // diffuse
   vec3 normal = normalize(u_normal_mat * extern_normal);
   vec3 light_direction = normalize(u_light.position - extern_frag_pos);
   float diffuse = max(dot(normal, light_direction), 0.0);
   vec3 diffuse_component = u_light.diffuse * diffuse * diffuse_map_color;

   // specular
   vec3 camera_direction = normalize(-u_camera_pos + extern_frag_pos);
   vec3 reflected_ray = reflect(light_direction, normal);
   float specular = pow(max(dot(camera_direction, reflected_ray), 0.0), u_material.shininess);
   vec3 specular_component =  u_light.specular * (specular * u_material.specular);

    vec3 result_color = ambient_component + diffuse_component + specular_component;
    out_color = vec4(result_color, 1.0);
}