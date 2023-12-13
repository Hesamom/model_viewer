#version 330 core
#include shadows.glsl
#include light.glsl

uniform sampler2DShadow u_shadowSampler;
uniform sampler2D u_diffuseSampler;

uniform vec3 u_light_color;
uniform material mat;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

void main()
{
    surface surf;
    surf.lightDir =  normalize(fs_in.lightDir);
    surf.viewDir = normalize(fs_in.viewDir);
    surf.normal = normalize(fs_in.normal);

    float shadow = getShadowValue(fs_in.fragPosLightSpace, surf.normal , surf.lightDir , u_shadowSampler);
    vec3 lighting = getLight(surf, u_light_color, shadow, mat);
    vec3 diffuseTextel = texture(u_diffuseSampler, fs_in.texCoord).rgb;
    vec3 color = lighting * diffuseTextel;

    FragColor = vec4(color, mat.opacity);
}