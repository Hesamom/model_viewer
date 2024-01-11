#version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2DShadow u_shadowSampler;
uniform sampler2D u_diffuseSampler;

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_light_dir;

uniform material mat;

uniform int u_pointLightCount = 0;
#define NR_POINT_LIGHTS 4
uniform pointLight u_pointLights[NR_POINT_LIGHTS];

uniform int u_spotLightCount = 0;
#define NR_SPOT_LIGHTS 4
uniform spotLight u_spotLights[NR_SPOT_LIGHTS];

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 viewDir;
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec4[4] fragSpotPosLightSpace;

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
    dirLight.direction = normalize(u_light_dir);
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
        vec3 lighting = getPointLight(surf, u_pointLights[i], shadow, mat);
        pointColors += lighting;;
    }

    //spot
    for (int i =0; i < u_spotLightCount; i++)
    {
        float shadow = getShadowValue(fs_in.fragSpotPosLightSpace[i], surf.normal , dirLight.direction , u_shadowSampler);
        vec3 lighting = getSpotLight(surf, u_spotLights[i], shadow, mat);
        pointColors += lighting;
    }

    vec3 diffuseTextel = texture(u_diffuseSampler, fs_in.texCoord).rgb;
    vec3 finalColor = pointColors * diffuseTextel;
    FragColor = vec4(finalColor, mat.opacity);
}