#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;
out vec4 out_color;
out vec3 out_normal;
out vec3 frag_position;

void main() {
  frag_position = vec3(model * vec4(position, 1.0));
  out_uv = uv;
  out_color = color;
  out_normal = mat3(transpose(inverse(model))) * normal;
  gl_Position = projection * view * vec4(frag_position, 1.0);
}

#shader fragment
#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dir_light;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NUM_POINT_LIGHTS 2
uniform PointLight point_lights[NUM_POINT_LIGHTS];

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 
  
out vec4 frag_color;

in vec2 out_uv;
in vec4 out_color;
in vec3 out_normal;
in vec3 frag_position;

uniform vec3 view_position;

uniform Material material;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main() {
  // properties
  vec3 norm = normalize(out_normal);
  vec3 view_dir = normalize(view_position - frag_position);

  // phase 1: Direction light
  vec3 result = calc_dir_light(dir_light, norm, view_dir);
  // phase 2: Point lights
  for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    result += calc_point_light(point_lights[i], norm, frag_position, view_dir);
  // phase 3: Spotlights?
  // Maybe.

  frag_color = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir) {
  vec3 light_dir = normalize(-light.direction);
  // diffuse shading
  float diff = max(dot(normal, light_dir), 0.0);
  // specular shading
  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  // combine results
  vec3 ambient  = light.ambient         * material.ambient * texture(diffuse_texture, out_uv).rgb;
  vec3 diffuse  = light.diffuse  * diff * material.diffuse * texture(diffuse_texture, out_uv).rgb;
  vec3 specular = light.specular * spec * material.specular * texture(specular_texture, out_uv).rgb; 
  return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
  vec3 light_dir = normalize(light.position - frag_pos);
  // diffuse shading
  float diff = max(dot(normal, light_dir), 0.0);
  // specular shading
  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  // attenuation
  float distance = length(light.position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  // combine results
  vec3 ambient  = light.ambient         * material.ambient * texture(diffuse_texture, out_uv).rgb;
  vec3 diffuse  = light.diffuse  * diff * material.diffuse * texture(diffuse_texture, out_uv).rgb;
  vec3 specular = light.specular * spec * material.specular * texture(specular_texture, out_uv).rgb; 
 
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}