#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;
out vec4 out_color;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  out_uv = uv;
  out_color = color;
}

#shader fragment
#version 330 core

out vec4 frag_color;

in vec2 out_uv;
in vec4 out_color;

uniform sampler2D ftex;

void main() {
  frag_color = texture(ftex, out_uv) * out_color;
}