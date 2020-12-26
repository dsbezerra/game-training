#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

out vec2 out_uv;

uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * vec4(position.xy, -position.z, 1.0));
  out_uv = uv;
}

#shader fragment
#version 330 core

out vec4 frag_color;
in vec2 out_uv;
uniform sampler2D ftex;

void main() {
  frag_color = texture(ftex, out_uv);
}