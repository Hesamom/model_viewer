   #version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2D u_diffuseSampler;
uniform sampler2D u_normalSampler;
uniform sampler2D u_depthSampler;
uniform sampler2DShadow u_shadowSampler;

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;

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
    vec3 lightDir;
    vec3 viewDir;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);


void main()
{
    vec2 texCoords = ParallaxMapping(fs_in.texCoord, fs_in.viewDir);

    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    surface surf;
    surf.viewDir = normalize(fs_in.viewDir);
    surf.normal = (texture(u_normalSampler, texCoords).rgb * 2.0) - 1.0;

    //dirct
    directLight dirLight;
    dirLight.direction = normalize(fs_in.lightDir);
    dirLight.ambient = u_lightAmbient;
    dirLight.diffuse = u_lightDiffuse;

    float shadow = getShadowValue(fs_in.fragPosLightSpace, surf.normal , dirLight.direction , u_shadowSampler);
    vec3 lighting = getDirLight(surf, dirLight, shadow, mat);
    vec3 diffuseTextel = texture(u_diffuseSampler, texCoords).rgb;
    vec3 dirColor = lighting * diffuseTextel;

    //point
    vec3 pointColors;
    for (int i =0; i < u_pointLightCount; i++)
    {
        //TODO add shadow here
        float shadow = 0;
        vec3 lighting = getPointLight(surf, u_pointLights[i], shadow, mat);
        pointColors += lighting * diffuseTextel;
    }

    //spot
    for (int i =0; i < u_spotLightCount; i++)
    {
        //TODO add shadow here
        float shadow = 0;
        vec3 lighting = getSpotLight(surf, u_spotLights[i], shadow, mat);
        pointColors += lighting;
    }

    vec3 finalColor = dirColor + pointColors;
    FragColor = vec4(finalColor, mat.opacity);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height_scale = 0.18f;
    float height =  texture(u_depthSampler, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}