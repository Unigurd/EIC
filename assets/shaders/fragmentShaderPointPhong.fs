#version 430 core

in vec3 norm;
in vec3 fragPos;

uniform vec3 color;
uniform vec3 dirLightDir;
uniform vec3 pointLightColor;
uniform vec3 pointLightPos;
uniform vec4 cameraPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform int alpha;

out vec4 FragColor;

void main()
{
    vec3 ambient = ka * pointLightColor;

    vec3 posLightDir = normalize(fragPos - pointLightPos);
    //vec3 posLightDir = normalize(pointLightPos - fragPos);

    float diff = max(dot(normalize(norm), -normalize(posLightDir)), 0.0);
    vec3 diffuse = kd * diff * pointLightColor;

    vec3 viewDir = normalize(cameraPos.xyz - fragPos);
    vec3 reflectDir = reflect(normalize(posLightDir), normalize(norm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), alpha);
    vec3 specular = ks * spec * pointLightColor;  
    //float specularStrength = 0.8;
    //vec3 viewDir = normalize(viewPos.xyz - fragPos);
    //vec3 reflectDir = normalize(reflect(normalize(viewLightDir), normalize(norm)));
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * spec * lightColor;  

    //vec3 result = ambient * color;
    //vec3 result = diffuse * color;
    //vec3 result = specular * color;
    vec3 result = (ambient + diffuse + specular) * color;
    //vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}
