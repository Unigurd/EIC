#version 430 core

in vec3 norm;
in vec3 viewLightDir;
uniform vec3 color;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;
out vec4 FragColor;

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    float diff = max(dot(normalize(norm), -normalize(viewLightDir)), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}
