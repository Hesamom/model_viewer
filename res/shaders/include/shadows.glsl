#define PCF


float getShadowValueIndexed(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2DArrayShadow shadowmaps, int index)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;

    #ifdef PCF
    const int area = 3;
    vec2 texelSize = 1.0 / textureSize(shadowmaps, 0).xy;
    float biasedDepth = currentDepth - bias;
    for(int x = -area/2; x <= area/2; ++x)
    {
        for(int y =  -area/2; y <= area/2; ++y)
        {
            float pcfDepth = texture(shadowmaps, vec4(projCoords.xy + vec2(x, y) * texelSize, index, biasedDepth));
            shadow += pcfDepth;
        }
    }
    shadow /= (area * area);
    #else
    float depth = texture(shadowmap, vec4(projCoords.xy, index, biasedDepth));
    shadow += depth;
    #endif


    return shadow;
}

float getShadowValue(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2DShadow shadowmap)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;

    #ifdef PCF
    const int area = 3;
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    float biasedDepth = currentDepth - bias;
    for(int x = -area/2; x <= area/2; ++x)
    {
        for(int y =  -area/2; y <= area/2; ++y)
        {
            float pcfDepth = texture(shadowmap, vec3(projCoords.xy + vec2(x, y) * texelSize, biasedDepth));
            shadow += pcfDepth;
        }
    }
    shadow /= (area * area);
    #else
    float depth = texture(shadowmap, vec3(projCoords.xy, biasedDepth));
    shadow += depth;
    #endif


    return shadow;
}