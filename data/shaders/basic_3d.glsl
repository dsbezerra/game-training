#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  out_uv = uv;
}

#shader fragment
#version 330 core

out vec4 frag_color;

in vec2 out_uv;

uniform sampler2D ftex;

void main() {
  vec4 sample = texture(ftex, out_uv);
  frag_color = vec4(sample.r, sample.r, sample.r, sample.r);
}