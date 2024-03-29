#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;

out VS_OUT {
  vec4 color;
} vs_out;

void main() {
  gl_Position = projection * view * vec4(position, 1.0);
  vs_out.color = color;
}

#shader fragment
#version 330 core

out vec4 frag_color;

in VS_OUT {
  vec4 color;
} fs_in;

void main() {
  frag_color = fs_in.color;
}
