   #version 330 core
#include shadows.glsl
#include light.glsl
   

uniform sampler2D u_depthSampler;
uniform sampler2DShadow u_shadowSampler;
uniform sampler2DArrayShadow u_spotShadowSamplers;

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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);


void main()
{
    vec3 viwDir = normalize(u_camera_pos - fs_in.fragPos);
    vec2 texCoords = ParallaxMapping(fs_in.texCoord, viwDir);

    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    surface surf;
    surf.viewDir = viwDir;
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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height_scale = 0.18f;
    float height =  texture(u_depthSampler, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}