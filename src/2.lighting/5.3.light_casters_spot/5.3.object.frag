#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cut_off;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 extern_normal;
in vec3 extern_frag_pos;
in vec2 extern_text_coords;

uniform Material u_material;
uniform SpotLight u_light;
uniform mat3 u_normal_mat;
uniform vec3 u_camera_pos;

out vec4 out_color;

void main()
{

    vec3 diffuse_map_color = vec3(texture(u_material.diffuse, extern_text_coords));
    vec3 specular_map_color = vec3(texture(u_material.specular, extern_text_coords));

    float distance = length(u_light.position - extern_frag_pos);
    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));
   vec3 normal = normalize(u_normal_mat * extern_normal);
   vec3 light_direction = normalize(u_light.position - extern_frag_pos);

    float theta = dot(light_direction, normalize(-u_light.direction));
    if (theta > u_light.cut_off) {
        // ambient
        vec3 ambient_component = u_light.ambient * diffuse_map_color;

       // diffuse
       float diffuse = max(dot(normal, light_direction), 0.0);
       vec3 diffuse_component = u_light.diffuse * diffuse * diffuse_map_color;

       // specular
       vec3 camera_direction = normalize(u_camera_pos - extern_frag_pos);
       vec3 reflected_ray = reflect(-light_direction, normal);
       float specular = pow(max(dot(camera_direction, reflected_ray), 0.0), u_material.shininess);
       vec3 specular_component = u_light.specular * specular * specular_map_color;

        ambient_component  *= attenuation;  
        diffuse_component   *= attenuation;
        specular_component *= attenuation;  

        vec3 result_color = ambient_component + diffuse_component + specular_component;
        out_color = vec4(result_color, 1.0);
    }
    else {
        out_color = vec4(
            u_light.ambient * vec3(texture(u_material.diffuse, extern_text_coords)),
            1.0);
    }

}
