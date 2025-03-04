#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
uniform mat4 model;
uniform mat4 viewProj;

uniform vec3 color;           
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

// See comments in fragmentShaderPhong.fs

out vec4 FragColor1;
void main()
{
    gl_Position = viewProj * model * vec4(aPos,1);
    vec3 norm = normalize(mat3(transpose(inverse(model))) * aNorm);
    vec3 fragPos = vec3(model * vec4(aPos, 1));

    vec3 viewDir = normalize(cameraPos.xyz - fragPos);

    float dirDiff = max(dot(norm, -normalize(dirLightDir)), 0.0);
    float dirDiffuse = kd * dirDiff;

    vec3 dirReflectDir = reflect(normalize(dirLightDir), norm);
    float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), alpha);
    float dirSpecular = ks * dirSpec;

    vec3 dirResult =  (dirSpecular + ((ka + dirDiffuse) * color))  * dirLightColor;

    vec3 pointLightDir = normalize(fragPos - pointLightPos);
    float d = length(pointLightDir);
    float att = 1 / (attenuation.z + d * attenuation.y + pow(d, 2) * attenuation.x);

    float pointDiff = max(dot(norm, -normalize(pointLightDir)), 0.0);
    float pointDiffuse = att * kd * pointDiff;

    vec3 pointReflectDir = reflect(normalize(pointLightDir), norm);
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), alpha);
    float pointSpecular = att * ks * pointSpec;

    vec3 pointResult = (pointSpecular + (pointDiffuse * color)) * pointLightColor;

    vec3 result = (dirResult + pointResult);
    FragColor1 = vec4(result, 1.0);
}
