#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 model;
uniform mat4 viewProj;

out vec3 norm;
out vec3 fragPos;
out vec2 TexCoord;
void main()
{
    gl_Position = viewProj * model * vec4(aPos,1);
    norm = mat3(transpose(inverse(model))) * aNorm;
    fragPos = vec3(model * vec4(aPos, 1));
    TexCoord = aTexCoord;
}
