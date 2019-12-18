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

    float dirDiff = max(dot(normalize(norm), -normalize(dirLightDir)), 0.0);
    vec3 dirDiffuse = kd * dirDiff * dirLightColor;

    vec3 viewDir = normalize(cameraPos.xyz - fragPos);
    vec3 dirReflectDir = reflect(normalize(dirLightDir), normalize(norm));
    float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), alpha);
    vec3 dirSpecular = ks * dirSpec * dirLightColor;  

    vec3 result = (ambient + dirDiffuse + dirSpecular) * color;
    FragColor = vec4(result, 1.0);
}
