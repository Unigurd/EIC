#version 430 core

in vec3 norm;
in vec3 fragPos;

uniform vec3 color;
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;
uniform vec4 cameraPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform int alpha;

out vec4 FragColor;

void main()
{
    vec3 ambient = ka * dirLightColor;

    float diff = max(dot(normalize(norm), -normalize(dirLightDir)), 0.0);
    vec3 diffuse = kd * diff * dirLightColor;

    vec3 viewDir = normalize(cameraPos.xyz - fragPos);
    vec3 reflectDir = reflect(normalize(dirLightDir), normalize(norm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), alpha);
    vec3 specular = ks * spec * dirLightColor;  
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
