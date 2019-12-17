#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
uniform mat4 model;
uniform mat4 viewProj;

out vec3 norm;
out vec3 fragPos;
void main()
{
    gl_Position = viewProj * model * vec4(aPos,1);
    norm = mat3(transpose(inverse(model))) * aNorm;
    fragPos = aPos;
}
