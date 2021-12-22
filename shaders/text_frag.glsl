#version 400 core

uniform sampler2D charSampler;

in vec2 texCoord;
out vec4 color;

void main()
{
   color = texture(charSampler, texCoord);
}
