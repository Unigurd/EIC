#version 430 core

in vec3 norm;
in vec3 fragPos;

// Properties of the object
uniform vec3 color;
uniform float ka;
uniform float kd;
uniform float ks;
uniform int alpha;

// Properties of the directional light
uniform vec3 dirLightColor;   
uniform vec3 dirLightDir;

// Properties of the point light
uniform vec3 pointLightColor;
uniform vec3 pointLightPos;
uniform vec4 cameraPos;
uniform vec3 attenuation;

out vec4 FragColor;

void main()
{
    vec3 nNorm = normalize(norm);

    // Vector from the camera to the fragment
    vec3 viewDir = normalize(cameraPos.xyz - fragPos);

    //
    // Phong shading for the directional light
    //

    // Diffuse component of the directional light
    float dirDiff = max(dot(nNorm, -normalize(dirLightDir)), 0.0);
    float dirDiffuse = kd * dirDiff;

    // Specular component of the directional light
    vec3 dirReflectDir = reflect(normalize(dirLightDir), nNorm);
    float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), alpha);
    float dirSpecular = ks * dirSpec;


    // The total directional lighting.
    // Note that ka, the ambient component, is present.
    // The specular component is not multiplied by the object color to gain
    // the white sheen present in the reference solution
    vec3 dirResult =  (dirSpecular + ((ka + dirDiffuse) * color))  * dirLightColor;

    //
    // Phong shading for the point light
    //

    // Direction of the point light
    vec3 pointLightDir = fragPos - pointLightPos;

    // Attenuation of the point light
    float d = length(pointLightDir);
    float att = 1 / (attenuation.z + d * attenuation.y + pow(d, 2) * attenuation.x);

    // Diffuse component of the point light
    float pointDiff = max(dot(nNorm, -normalize(pointLightDir)), 0.0);
    float pointDiffuse = att * kd * pointDiff;

    // Specular component of the point light
    vec3 pointReflectDir = reflect(normalize(pointLightDir), nNorm);
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), alpha);
    float pointSpecular = att * ks * pointSpec;

    // The total point lighting.
    // Note that ka, the ambient component, is not present.
    // If both lights had ambient components the objects appeared too light,
    // even if both used ka/2 instead. (Since the point light is brighter).
    // The specular component is not multiplied by the object color to gain 
    // the white sheen present in the reference solution
    vec3 pointResult = (pointSpecular + (pointDiffuse * color)) * pointLightColor;

    // Final color of the fragment.
    vec3 result = (dirResult + pointResult);
    FragColor = vec4(result, 1.0);
}
