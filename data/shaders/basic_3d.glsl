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

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 
  
uniform Material material;
uniform vec3 view_position;

uniform sampler2D diffuse_texture;
uniform bool blinn;

out vec4 frag_color;

in vec4 out_color;
in vec2 out_uv;
in vec3 out_normal;
in vec3 frag_position;

vec3 gamma_correct(vec3 color) {
  float gamma = 2.2;
  return pow(color, vec3(1.0/gamma));
}

vec3 light_color = vec3(1.0, 1.0, 1.0);

void main() {
  vec3 normal = normalize(out_normal);

  vec3 view_dir = normalize(view_position - frag_position);
  vec3 light_dir = vec3(-0.2, -1.0, -0.3);
  light_dir = normalize(-light_dir);
  
  // diffuse shading
  float diff = max(dot(normal, light_dir), 0.0);
  
  // specular shading
  float spec = 0.0;
  if (blinn) {
    vec3 halfway_dir = normalize(light_dir + view_dir);
    spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);
  } else {
    vec3 reflect_dir = reflect(-light_dir, normal);
    spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess / 2.0);
  }
  // combine results
  vec3 ambient  = light_color * material.diffuse * vec3(0.2) * texture(diffuse_texture, out_uv).rgb;
  vec3 diffuse  = light_color * diff * material.diffuse * texture(diffuse_texture, out_uv).rgb;
  vec3 specular = light_color * spec * material.specular;
  
  vec3 final_color = (ambient + diffuse + specular);  

  vec3 color = gamma_correct(final_color);
  frag_color = vec4(color, 1.0);
}
