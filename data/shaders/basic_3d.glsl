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
  vec3 frag_position;
  vec3 normal;
  vec4 color;
  vec2 uv;
  vec4 frag_position_in_light_space;
} vs_out;

void main() {
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

uniform bool blinn;

out vec4 frag_color;

in VS_OUT {
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

float calc_shadow(vec3 light_dir, vec4 fragPosLightSpace) {

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if (projCoords.z > 1.0)
      return 0.0;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, light_dir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
      for(int y = -1; y <= 1; ++y)
      {
        float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
      }    
    }
    shadow /= 9.0;
    return shadow;
}

vec3 light_color = vec3(1.0, 1.0, 1.0);

void main() {
  vec3 material_diffuse_color = texture(diffuse_texture, fs_in.uv).rgb;
  vec3 material_ambient_color = vec3(0.2) * material_diffuse_color;

  vec3 n = normalize(fs_in.normal);
  vec3 l = normalize(vec3(0.0, 1.0, 0.0));
 
  
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
  float diff = max(dot(n, l), 0.0);
  
  // combine results
  vec3 ambient  = material_ambient_color;
  vec3 diffuse  = light_color * diff * material_diffuse_color;
  vec3 specular = light_color * spec * material.specular;
  
  float shadow = calc_shadow(l, fs_in.frag_position_in_light_space);
  vec3 final_color = (ambient + (1.0 - shadow) * (diffuse + specular));  

  vec3 color = gamma_correct(final_color);
  frag_color = vec4(color, 1.0);
}
