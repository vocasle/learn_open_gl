#version 330 core

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 frag_color;

in vec2 text_coords;
in vec3 frag_pos;
in vec3 normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform DirLight dir_light;
uniform PointLight point_light;
uniform vec3 view_pos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
	vec3 norm = normalize(normal);
    vec3 view_dir = normalize(view_pos - frag_pos);
    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dir_light, norm, view_dir);
    // phase 2: Point lights
    result += CalcPointLight(point_light, norm, frag_pos, view_dir);

    frag_color = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir)
{
	vec3 light_dir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(light_dir, normal), 0.0);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, text_coords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, text_coords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, text_coords));
	return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(light.position - frag_pos);
	// diffuse shading
	float diff = max(dot(light_dir, normal), 0.0);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0f);
	// attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, text_coords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, text_coords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, text_coords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}