#version 330 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vTex;

out vec2 texCoord;

void main()
{
    texCoord = vTex;
    gl_Position = vec4(vPos, 0, 1.0);
}