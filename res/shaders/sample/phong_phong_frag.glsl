#version 330 core

uniform sampler2DShadow u_shadowSampler;
uniform sampler2D u_diffuseSampler;


uniform vec3 u_light_color;

uniform vec3 u_ambient;
uniform vec3 u_diffuseAlbedo;
uniform vec3 u_specularAlbedo;
uniform float u_shininess;
uniform float u_opacity = 1;

#define PCF

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

float getShadowValue(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(fs_in.normal, fs_in.lightDir)), 0.0005);
    
    float shadow = 0.0;
    
    #ifdef PCF
    const int area = 3;
    vec2 texelSize = 1.0 / textureSize(u_shadowSampler, 0);
    float biasedDepth = currentDepth - bias;
    for(int x = -area/2; x <= area/2; ++x)
    {
        for(int y =  -area/2; y <= area/2; ++y)
        {
            float pcfDepth = texture(u_shadowSampler, vec3(projCoords.xy + vec2(x, y) * texelSize, biasedDepth));
            shadow += pcfDepth;
        }
    }
    shadow /= (area * area);
    #else
      float depth = texture(u_shadowSampler, vec3(projCoords.xy, biasedDepth));
      shadow += depth;
    #endif
    
    
    return shadow;
}

void main()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(fs_in.lightDir);
    vec3 viewDir = normalize(fs_in.viewDir);

    vec3 lightReflection = reflect(-lightDir, normal);

    //calculate light components 

    vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_diffuseAlbedo;
    vec3 specular = pow(max(dot(lightReflection, viewDir), 0.0), u_shininess) * u_specularAlbedo;
    vec3 ambient = (u_ambient * u_light_color);
    vec3 diffuseTextel = texture(u_diffuseSampler, fs_in.texCoord).rgb;
    
    float shadow = getShadowValue(fs_in.fragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseTextel;

    FragColor = vec4(lighting, u_opacity);
}