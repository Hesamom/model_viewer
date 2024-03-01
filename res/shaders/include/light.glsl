#define NR_SPOT_LIGHTS 4
#define NR_POINT_LIGHTS 4
#define SPEC_BLIN;

struct material
{
    vec3 ambient;
    vec3 diffuseAlbedo;
    vec3 specularAlbedo;
    float shininess;
    float opacity;
    float reflectivity;

    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
};

struct surface
{
    vec3 viewDir;
    vec3 normal;
    vec3 fragPos;
    
    vec3 specTexel;
    vec3 diffuseTexel;
};

struct directLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

struct pointLight
{
    vec3 position;
    float range;
    float insideRange;
    
    vec3 ambient;
    vec3 diffuse;
};

struct spotLight
{
    vec3 position;
    vec3 direction;
    
    float innerCutoff;
    float outerCutoff;
    
    vec3 ambient;
    vec3 diffuse;
};

struct baseLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 toSourceDirection;
    float intensity;
};


float getSpecular(surface surf, baseLight light)
{
    #ifdef SPEC_BLIN
        vec3 halfVec = normalize(surf.viewDir + light.toSourceDirection);
        float spec = max(dot(surf.normal, halfVec), 0.0);
        return spec;
    #else
        vec3 lightReflection = reflect(-light.toSourceDirection, surf.normal);
        float spec = max(dot(lightReflection, surf.viewDir), 0.0);
        return spec;
    #endif
}

vec3 computeLight(surface surf, float shadow, material mat, baseLight light)
{
    vec3 diffuse = max(dot(surf.normal, light.toSourceDirection), 0.0) * mat.diffuseAlbedo * light.diffuse * light.intensity * surf.diffuseTexel;
    
    float spec = getSpecular(surf, light);
    vec3 specular = pow(spec, mat.shininess) * mat.specularAlbedo * light.diffuse * light.intensity * surf.specTexel;
    //do we really need a different ambient color both per light and object?
    vec3 ambient = mat.ambient * light.ambient * light.intensity * surf.diffuseTexel;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    return lighting;
}

vec3 getPointLight(surface surf, pointLight pLight, float shadow, material mat)
{
    vec3 lightDir = pLight.position - surf.fragPos;
    vec3 lightDirNormalized = normalize(lightDir);
    float range = length(lightDir);
    float attenuation = 1 - pow(max(range, pLight.insideRange),2)/(pLight.range * pLight.range);

    baseLight light;
    light.intensity = attenuation;
    light.toSourceDirection = lightDirNormalized;
    light.ambient = pLight.ambient;
    light.diffuse = pLight.diffuse;
    
    return computeLight(surf, shadow, mat, light);
}

vec3 getDirLight(surface surf, directLight directlight, float shadow, material mat)
{
    baseLight light;
    light.intensity = 1;
    light.toSourceDirection = directlight.direction;
    light.ambient = directlight.ambient;
    light.diffuse = directlight.diffuse;

    return computeLight(surf, shadow, mat, light);
}

vec3 getSpotLight(surface surf, spotLight sLight, float shadow, material mat)
{
    vec3 lightDir = normalize(sLight.position - surf.fragPos);
    float theta     = dot(lightDir, normalize(-sLight.direction));
    float epsilon   = sLight.innerCutoff - sLight.outerCutoff;
    float intensity = clamp((theta - sLight.outerCutoff) / epsilon, 0.0, 1.0);
    
    baseLight light;
    light.intensity = intensity;
    light.toSourceDirection = lightDir;
    light.ambient = sLight.ambient;
    light.diffuse = sLight.diffuse;

    return computeLight(surf, shadow, mat, light);
}

vec3 computePointLights(surface surf, int lightCount, pointLight lights[NR_POINT_LIGHTS], material mat)
{
    vec3 color;
    for (int i =0; i < lightCount; i++)
    {
        //TODO add shadow here
        float shadow = 0;
        vec3 lighting = getPointLight(surf, lights[i], shadow, mat);
        color += lighting;
    }
    
    return color;
}

vec3 computeSpotLights(surface surf, int lightCount, spotLight lights[NR_SPOT_LIGHTS], vec4 lightSpace[NR_SPOT_LIGHTS], sampler2DArrayShadow sampler, material mat)
{
    vec3 color;
    for (int i =0; i < lightCount; i++)
    {
        vec3 lightDir = surf.fragPos - lights[i].position;
        float shadow = getShadowValueIndexed(lightSpace[i], surf.normal , lightDir , sampler, i);
        vec3 lighting = getSpotLight(surf, lights[i], shadow, mat);
        color += lighting;
    }
    return color;
}