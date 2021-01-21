#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;

out VS_OUT {
  vec3 position;
  vec3 frag_position;
  vec3 normal;
  vec4 color;
  vec2 uv;
  vec4 frag_position_in_light_space;
} vs_out;

void main() {
  vs_out.position = position;
  vs_out.frag_position = vec3(model * vec4(position, 1.0));
  vs_out.normal = mat3(transpose(inverse(model))) * normal;
  vs_out.uv = uv;
  vs_out.color = color;
  vs_out.frag_position_in_light_space = light_space_matrix * vec4(vs_out.frag_position, 1.0);
  gl_Position = projection * view * vec4(vs_out.frag_position, 1.0);
}

#shader fragment
#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 
  
uniform Material material;
uniform vec3 view_position;

uniform sampler2D diffuse_texture;
uniform sampler2D shadow_map;
uniform vec3 light_pos;
uniform bool blinn;

vec3 light_color = vec3(0.8, 0.598313, 0.000746); // Matches the blender file material color

out vec4 frag_color;

in VS_OUT {
  vec3 position;
  vec3 frag_position;
  vec3 normal;
  vec4 color;
  vec2 uv;
  vec4 frag_position_in_light_space;
} fs_in;

vec3 gamma_correct(vec3 color) {
  float gamma = 2.2;
  return pow(color, vec3(1.0/gamma));
}

float HardShadows_DirectionalLight(vec3 ShadowCoords) {
  if (ShadowCoords.z > 1.0)
    return 0.0;

  ShadowCoords = ShadowCoords * 0.5 + 0.5;

  vec3 l = normalize(light_pos - fs_in.frag_position);
  float bias = max(0.01 * (1.0 - dot(fs_in.normal, l)), 0.005);

  float z = texture(shadow_map, ShadowCoords.xy).x;
  return (z < (ShadowCoords.z - bias)) ? 1.0 : 0.0;
}

void main() {
  vec3 material_diffuse_color = texture(diffuse_texture, fs_in.uv).rgb;
  vec3 material_ambient_color = vec3(0.1) * material_diffuse_color;

  vec3 n = normalize(fs_in.normal);
  vec3 l = normalize(light_pos - fs_in.frag_position);

  vec3 E = normalize(view_position - fs_in.frag_position);
  
  // specular shading
  float spec = 0.0;
  if (blinn) {
    vec3 halfway_dir = normalize(l + E);
    spec = pow(max(dot(n, halfway_dir), 0.0), material.shininess);
  } else {
    vec3 R = reflect(-l, n);
    spec = pow(max(dot(E, R), 0.0), material.shininess / 2.0);
  }
  
  // diffuse shading
  float diff = max(dot(l, n), 0.0);
  
  // combine results
  vec3 ambient  = material_ambient_color;
  vec3 diffuse  = light_color * diff * material_diffuse_color;
  vec3 specular = light_color * spec * material.specular;
  
  vec3 ShadowCoords = fs_in.frag_position_in_light_space.xyz / fs_in.frag_position_in_light_space.w;

  float shadow = HardShadows_DirectionalLight(ShadowCoords);
  vec3 final_color;
  if (material.diffuse == light_color) {
    final_color = material_diffuse_color * light_color;
  } else {
    final_color = (ambient + (1.0 - shadow) * (diffuse + specular));
  }   

  vec3 color = gamma_correct(final_color);
  frag_color = vec4(color, 1.0);
}