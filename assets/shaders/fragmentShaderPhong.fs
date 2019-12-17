#version 430 core

in vec3 norm;
in vec3 viewLightDir;
in vec3 fragPos;

uniform vec3 color;
uniform vec3 lightColor;
uniform vec4 viewPos;
uniform float ka;
uniform float kd;
uniform float ks;

out vec4 FragColor;

void main()
{
    vec3 ambient = ka * lightColor;

    float diff = max(dot(normalize(norm), -normalize(viewLightDir)), 0.0);
    vec3 diffuse = kd * diff * lightColor;

    vec3 viewDir = normalize(viewPos.xyz - fragPos);
    vec3 reflectDir = reflect(normalize(viewLightDir), normalize(norm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
    vec3 specular = ks * spec * lightColor;  
    //float specularStrength = 0.8;
    //vec3 viewDir = normalize(viewPos.xyz - fragPos);
    //vec3 reflectDir = normalize(reflect(normalize(viewLightDir), normalize(norm)));
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * spec * lightColor;  

    //vec3 result = (specular) * color;
    vec3 result = (ambient + diffuse + specular) * color;
    //vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}
