#version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;
uniform sampler2D normalSampler;
uniform samplerCube u_reflectionSampler;

#if SHADOW
uniform sampler2DShadow u_shadowSampler;
uniform sampler2DArrayShadow u_spotShadowSamplers;
#endif

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_light_dir;
uniform vec3 u_camera_pos;

layout(std140) uniform u_mat
{
    material mat;
};

uniform int u_pointLightCount = 0;

uniform pointLight u_pointLights[NR_POINT_LIGHTS];

uniform int u_spotLightCount = 0;
uniform spotLight u_spotLights[NR_SPOT_LIGHTS];

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec4[4] fragSpotPosLightSpace;

} fs_in;

out vec4 FragColor;

void main()
{
    surface surf;
    surf.fragPos = fs_in.fragPos;
    surf.viewDir = normalize(u_camera_pos - fs_in.fragPos);
    surf.normal = normalize(fs_in.normal);
    surf.specTexel = texture(specularSampler, fs_in.texCoord).rgb;
    surf.diffuseTexel = texture(diffuseSampler, fs_in.texCoord).rgb;
    
    directLight dirLight;
    dirLight.direction = normalize(u_light_dir);
    dirLight.ambient = u_lightAmbient;
    dirLight.diffuse = u_lightDiffuse;

    float shadow = 0;
    #if SHADOW
        shadow = getShadowValue(fs_in.fragPosLightSpace, surf.normal , dirLight.direction , u_shadowSampler);
    #endif
    
    vec3 finalColor =  getDirLight(surf, dirLight, shadow, mat);
    
    finalColor += computePointLights(surf, u_pointLightCount, u_pointLights, mat);
    //finalColor += computeSpotLights(surf, u_spotLightCount, u_spotLights, fs_in.fragSpotPosLightSpace, u_spotShadowSamplers, mat);
    
    if(mat.reflectivity > 0.1f)
    {
        //todo can take the reflection computation to vertex shader to improve perf
        vec3 reflection = reflect(-surf.viewDir, surf.normal);
        vec3 reflectionColor = texture(u_reflectionSampler, reflection).xyz;
        finalColor = mix(finalColor, reflectionColor , mat.reflectivity);
    }
    
    FragColor = vec4(finalColor,  mat.opacity);
}