#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in float z_index;

uniform mat4 projection;
uniform mat4 view;
out vec4 out_color;
out vec2 out_uv;
out float out_z_index;

void main() {
  gl_Position = projection * view * vec4(position, 1.0);
  out_color = color;
  out_uv = uv;
  out_z_index = z_index;
}

#shader fragment
#version 330 core

out vec4 frag_color;
in vec4 out_color;
in vec2 out_uv;
in float out_z_index;
uniform sampler2D ftex;

void main() {
  if (out_uv.x < 0 && out_uv.y < 0) frag_color = out_color;
  else {
    vec4 sample = texture(ftex, out_uv);
    if (out_z_index < 0.0) { // @Hack to make the same shader work for text and textures.
      frag_color = vec4(out_color.xyz, sample.r);
    } else {
      frag_color = sample;
    }
 }
}
