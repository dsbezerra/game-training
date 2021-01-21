#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;

out vec4 out_color;

void main() {
  gl_Position = projection * view * vec4(position, 1.0);
  out_color = color;
}

#shader fragment
#version 330 core

out vec4 frag_color;
in vec4 out_color;

void main() {
  frag_color = out_color;
}
