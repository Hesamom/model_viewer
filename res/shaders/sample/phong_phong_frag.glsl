#version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2DShadow u_shadowSampler;
uniform sampler2D u_diffuseSampler;

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;

uniform material mat;

uniform int u_pointLightCount = 0;
#define NR_POINT_LIGHTS 4
uniform pointLight pointLights[NR_POINT_LIGHTS];

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    vec3 fragPos;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

void main()
{
    surface surf;
    surf.fragPos = fs_in.fragPos;
    surf.viewDir = normalize(fs_in.viewDir);
    surf.normal = normalize(fs_in.normal);

    //dirct
    directLight dirLight;
    dirLight.direction = normalize(fs_in.lightDir);
    dirLight.ambient = u_lightAmbient;
    dirLight.diffuse = u_lightDiffuse;
    
    float shadow = getShadowValue(fs_in.fragPosLightSpace, surf.normal , dirLight.direction , u_shadowSampler);
    vec3 lighting = getDirLight(surf, dirLight, shadow, mat);
    vec3 dirColor = lighting;
    
    //point
    vec3 pointColors = dirColor;
    for (int i =0; i < u_pointLightCount; i++)
    {
        //TODO add shadow here
        float shadow = 0;
        vec3 lighting = getPointLight(surf, pointLights[i], shadow, mat);
        pointColors += lighting;;
    }

    vec3 diffuseTextel = texture(u_diffuseSampler, fs_in.texCoord).rgb;
    vec3 finalColor = pointColors * diffuseTextel;
    FragColor = vec4(finalColor, mat.opacity);
}