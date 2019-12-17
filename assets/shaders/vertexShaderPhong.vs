#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
uniform mat4 model;
uniform mat4 viewProj;
uniform vec3 lightDir;

out vec3 norm;
out vec3 viewLightDir;
out vec3 fragPos;
void main()
{
    gl_Position = viewProj * model * vec4(aPos,1);
    norm = mat3(transpose(inverse(model))) * aNorm;
    viewLightDir = lightDir;
    fragPos = aPos;
    //norm = mat3(viewProj) * mat3(transpose(inverse(model))) * aNorm;
    //viewLightDir = mat3(viewProj) * lightDir;
}
