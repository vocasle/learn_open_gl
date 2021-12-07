#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material u_material;
uniform Light u_light;
uniform vec3 u_camera_pos;

in vec4 extern_normal;
in vec4 extern_frag_pos;

out vec4 out_color;

void main()
{
    // ambient
    vec3 ambient = u_light.ambient * u_material.ambient;
    
    // diffuse
    vec4 normal = normalize(extern_normal);
    vec4 light_dir = normalize(vec4(u_light.position, 1.0) - extern_frag_pos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = u_light.diffuse * (diff * u_material.diffuse);

    // specular
    vec4 view_dir = normalize(vec4(u_camera_pos, 1.0) - extern_frag_pos);
    vec4 reflected_view_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflected_view_dir), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * (spec * u_material.specular);
    out_color = vec4(ambient + diffuse + specular, 1.0);
}