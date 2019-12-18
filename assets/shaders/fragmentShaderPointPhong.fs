#version 430 core

in vec3 norm;
in vec3 fragPos;

uniform vec3 color;
uniform vec3 dirLightDir; // can probably be deleted
uniform vec3 pointLightColor;
uniform vec3 pointLightPos;
uniform vec4 cameraPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform int alpha;
uniform float attConst;
uniform float attLin;
uniform float attQuad;

out vec4 FragColor;

void main()
{
    vec3 ambient = ka * pointLightColor;

    vec3 pointLightDir = normalize(fragPos - pointLightPos);
    float d = length(pointLightDir);
    float attenuation = 1 / (attConst + d * attLin + pow(d, 2) * attQuad);

    float pointDiff = max(dot(normalize(norm), -normalize(pointLightDir)), 0.0);
    vec3 pointDiffuse = attenuation * kd * pointDiff * pointLightColor;

    vec3 viewDir = normalize(cameraPos.xyz - fragPos);
    vec3 pointReflectDir = reflect(normalize(pointLightDir), normalize(norm));
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), alpha);
    vec3 pointSpecular = attenuation * ks * pointSpec * pointLightColor;  

    vec3 result = (ambient + pointDiffuse + pointSpecular) * color;
    FragColor = vec4(result, 1.0);
}
