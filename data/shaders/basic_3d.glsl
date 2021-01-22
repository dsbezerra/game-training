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

int samples = 1;
vec2 poisson_disk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 )
);

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
  float bias = max(0.001 * (1.0 - dot(fs_in.normal, l)), 0.001);
  

  float visibility = 0.0;
  for (int index = 0; index < samples; index++) {    
    float z = texture(shadow_map, ShadowCoords.xy + poisson_disk[index] / 700.0).x;
    if (z < ShadowCoords.z - bias) {
      visibility += 1.0 / samples;
    }
  }

  return visibility;
}

// Uses PCF
float SoftShadows_DirectionalLight(vec3 ShadowCoords) {
  if (ShadowCoords.z > 1.0)
    return 0.0;

  ShadowCoords = ShadowCoords * 0.5 + 0.5;

  vec3 l = normalize(light_pos - fs_in.frag_position);
  //float bias = max(0.01 * (1.0 - dot(fs_in.normal, l)), 0.005);
  float bias = max(0.001 * (1.0 - dot(fs_in.normal, l)), 0.001);

  float shadow = 0.0;
  vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcf = texture(shadow_map, ShadowCoords.xy + vec2(x, y) * texel_size).r; 
      shadow += ShadowCoords.z - bias > pcf ? 1.0 : 0.0;        
    }    
  }
  shadow /= 9.0;
  return shadow;
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

  float shadow = SoftShadows_DirectionalLight(ShadowCoords);
  vec3 final_color;
  if (material.diffuse == light_color) {
    final_color = material_diffuse_color * light_color;
  } else {
    final_color = (ambient + (1.0 - shadow) * (diffuse + specular));
  }   

  vec3 color = gamma_correct(final_color);
  frag_color = vec4(color, 1.0);
}