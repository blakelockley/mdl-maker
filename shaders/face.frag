#version 330 core

in vec3 out_frag_pos;
in vec3 out_normal; 

out vec4 out_frag_color;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_color;

void main()
{
    vec3 norm = normalize(out_normal);
    vec3 light_dir = normalize(light_pos - out_frag_pos);

    float amb = 0.25;
    vec3 ambient = amb * light_color;

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;
    
    vec3 result = (ambient + diffuse) * color;
    out_frag_color = vec4(result, 1.0);
}
