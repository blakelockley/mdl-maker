#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 id_color;

uniform mat4 mvp;

out vec3 out_color;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    out_color = id_color;
}
