#version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2DShadow u_shadowSampler;
uniform sampler2DArrayShadow u_spotShadowSamplers;

//consider defining a struct for these:
uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_light_dir;

uniform vec3 u_camera_pos;

uniform material u_mat;

uniform int u_pointLightCount = 0;
uniform pointLight u_pointLights[NR_POINT_LIGHTS];

uniform int u_spotLightCount = 0;
uniform spotLight u_spotLights[NR_SPOT_LIGHTS];

in VS_OUT
{
    vec2 texCoord;
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec4[4] fragSpotPosLightSpace;
    mat3 TBN;

} fs_in;

out vec4 FragColor;



void main()
{
    surface surf;
    surf.viewDir = normalize(u_camera_pos - fs_in.fragPos);
    surf.normal = fs_in.TBN * ((texture(u_mat.normalSampler, fs_in.texCoord).rgb * 2.0) - 1.0);
    surf.specTexel = texture(u_mat.specularSampler, fs_in.texCoord).rgb;
    surf.diffuseTexel = texture(u_mat.diffuseSampler, fs_in.texCoord).rgb;
    surf.fragPos = fs_in.fragPos;
    
    //dirct
    directLight dirLight;
    dirLight.direction = normalize(u_light_dir);
    dirLight.ambient = u_lightAmbient;
    dirLight.diffuse = u_lightDiffuse;
    
    float shadow = getShadowValue(fs_in.fragPosLightSpace, surf.normal , dirLight.direction , u_shadowSampler);
    vec3 finalColor =  getDirLight(surf, dirLight, shadow, u_mat);

    finalColor += computePointLights(surf, u_pointLightCount, u_pointLights, u_mat);
    finalColor += computeSpotLights(surf, u_spotLightCount, u_spotLights, fs_in.fragSpotPosLightSpace, u_spotShadowSamplers, u_mat);

    FragColor = vec4(finalColor, u_mat.opacity);
}