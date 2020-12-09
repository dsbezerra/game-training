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

struct Light {
    vec3 position;
  
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
  
out vec4 frag_color;

in vec2 out_uv;
in vec4 out_color;
in vec3 out_normal;
in vec3 frag_position;

uniform vec3 view_position;

uniform Light light;
uniform Material material;

void main() {
    // ambient
    vec4 ambient = light.ambient * texture(material.diffuse, out_uv);
    
    // diffuse
    vec3 norm = normalize(out_normal);
    vec3 light_dir = normalize(light.position - frag_position);
    float diff = max(dot(norm, light_dir), 0.0);
    vec4 diffuse = light.diffuse * diff * texture(material.diffuse, out_uv);
    
    // specular
    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec4 specular = light.specular * spec * texture(material.specular, out_uv);

    vec4 result = (ambient + diffuse + specular);
    frag_color = vec4(result.rgb, 1.0);
}