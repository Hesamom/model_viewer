#version 330 core

uniform sampler2D u_diffuseSampler;
uniform sampler2D u_shadowSampler;

uniform vec3 u_light_color;

uniform vec3 u_ambient;
uniform vec3 u_diffuseAlbedo;
uniform vec3 u_specularAlbedo;
uniform float u_shininess;
uniform float u_opacity = 1;

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
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowSampler, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(fs_in.normal, fs_in.lightDir)), 0.0005);


    float shadow = 0.0;
    const int area = 3;
    vec2 texelSize = 1.0 / textureSize(u_shadowSampler, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowSampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (area * area);
    
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