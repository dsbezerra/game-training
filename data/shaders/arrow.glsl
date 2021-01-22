#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

uniform vec4 diffuse_color;

out vec4 frag_color;

void main() {
  frag_color = diffuse_color;
}