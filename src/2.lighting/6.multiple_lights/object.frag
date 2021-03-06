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

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cut_off;
	float outer_cut_off;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec3 normal;
in vec3 frag_pos;
in vec2 text_coords;

out vec4 frag_color;

#define NR_POINT_LIGHTS 4

uniform DirLight dir_light;
uniform PointLight[NR_POINT_LIGHTS] point_lights;
uniform Material material;
uniform vec3 view_pos;
uniform SpotLight spot_light;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(view_pos - frag_pos);
	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dir_light, norm, view_dir);
	// phase 2: Point lights
	for (int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += CalcPointLight(point_lights[i], norm, frag_pos, view_dir);
	// phase 3: Spot light
	result += CalcSpotLight(spot_light, norm, frag_pos, view_dir);
	
	frag_color = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir)
{
	vec3 light_dir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(light_dir, normal), 0.0);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, text_coords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, text_coords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, text_coords));
	return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(light.position - frag_pos);
	// diffuse shading
	float diff = max(dot(light_dir, normal), 0.0);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, text_coords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, text_coords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, text_coords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	// ambient shading
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, text_coords));
	// diffuse shading
	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light.position - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, text_coords));

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, text_coords));

	// spotlight soft edges
	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.outer_cut_off - light.cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	// attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}





















