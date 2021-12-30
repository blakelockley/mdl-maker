#version 330 core

in vec3 FragPos;
in vec3 Normal; 

out vec4 FragColor;

uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float amb = 0.25;
    vec3 ambient = amb * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}
