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

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  out_uv = uv;
  out_color = color;
  out_normal = normal;
}

#shader fragment
#version 330 core

out vec4 frag_color;

in vec2 out_uv;
in vec4 out_color;
in vec3 out_normal;

uniform sampler2D ftex;
uniform vec4 light_color;

void main() {
   frag_color = out_color;
}