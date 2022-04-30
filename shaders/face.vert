#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 out_frag_pos;
out vec3 out_normal;

uniform mat4 model, view, projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    out_frag_pos = vec3(model * vec4(pos, 1.0));
    out_normal = normal;
}
