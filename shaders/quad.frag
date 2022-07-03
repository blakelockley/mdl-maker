#version 330 core

in vec2 a_uv;

uniform vec4 color;
uniform vec2 size;
uniform float radius;

out vec4 out_color;

void main()
{    
    float r = radius;
    vec2 coords = a_uv * size;
    
    vec2 xy = max(vec2(r) - coords, coords - size + vec2(r));
    float d = (length(max(vec2(0), xy)) - r) + min(0.0, max(xy.x, xy.y));

    float alpha = 1 - smoothstep(0, 1, d);

    out_color = vec4(color.rgb, alpha);
}  