#version 430 core

in vec3 norm;
in vec3 fragPos;

uniform vec3 color;
uniform vec3 dirColor;
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;
uniform vec3 pointLightColor;
uniform vec3 pointLightPos;
uniform vec4 cameraPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform int alpha;
uniform vec3 attenuation;

out vec4 FragColor;

void main()
{
    vec3 ambient = ka * dirLightColor;
    vec3 viewDir = normalize(cameraPos.xyz - fragPos);

    float dirDiff = max(dot(normalize(norm), -normalize(dirLightDir)), 0.0);
    float dirDiffuse = kd * dirDiff;

    vec3 dirReflectDir = reflect(normalize(dirLightDir), normalize(norm));
    float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), alpha);
    float dirSpecular = ks * dirSpec;

    vec3 dirTotal = (dirDiffuse + dirSpecular)  * dirLightColor;

    vec3 pointLightDir = fragPos - pointLightPos;
    float d = length(pointLightDir);
    float att = 1 / (attenuation.z + d * attenuation.y + pow(d, 2) * attenuation.x);

    float pointDiff = max(dot(normalize(norm), -normalize(pointLightDir)), 0.0);
    float pointDiffuse = att * kd * pointDiff;

    vec3 pointReflectDir = reflect(normalize(pointLightDir), normalize(norm));
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), alpha);
    float pointSpecular = att * ks * pointSpec;

    vec3 pointTotal = (pointDiffuse + pointSpecular)  * pointLightColor;

    vec3 result = (ambient + dirTotal + pointTotal) * color;
    FragColor = vec4(result, 1.0);
}
