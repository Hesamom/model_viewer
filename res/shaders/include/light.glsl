struct material
{
    vec3 ambient;
    vec3 diffuseAlbedo;
    vec3 specularAlbedo;
    float shininess;
    float opacity;
};

struct surface
{
    vec3 viewDir;
    vec3 normal;
    vec3 fragPos;
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



vec3 getPointLight(surface surf, pointLight light, float shadow, material mat)
{
    vec3 lightDir = light.position - surf.fragPos;
    vec3 lightDirNormalized = normalize(lightDir);
    float range = length(lightDir);
    
    vec3 lightReflection = reflect(-lightDirNormalized, surf.normal);
    float attenuation = 1 - pow(max(range, light.insideRange),2)/(light.range * light.range);
    
    vec3 diffuse = max(dot(surf.normal, lightDirNormalized), 0.0) * mat.diffuseAlbedo * light.diffuse * attenuation;
    vec3 specular = pow(max(dot(lightReflection, surf.viewDir), 0.0), mat.shininess) * mat.specularAlbedo * attenuation;
    vec3 ambient = (mat.ambient * light.ambient) * attenuation;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse));
    return lighting;
}

vec3 getDirLight(surface surf, directLight light, float shadow, material mat)
{
    vec3 lightReflection = reflect(-light.direction, surf.normal);

    //calculate light components
    vec3 diffuse = max(dot(surf.normal, light.direction), 0.0) * mat.diffuseAlbedo * light.diffuse;
    vec3 specular = pow(max(dot(lightReflection, surf.viewDir), 0.0), mat.shininess) * mat.specularAlbedo * light.diffuse;
    vec3 ambient = (mat.ambient * light.ambient);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    return lighting;
}

vec3 getSpotLight(surface surf, spotLight light, float shadow, material mat)
{
    vec3 lightDir = normalize(light.position - surf.fragPos);
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    vec3 lightReflection = reflect(-lightDir, surf.normal);
    
    //calculate light components
    vec3 diffuse =  max(dot(surf.normal, lightDir), 0.0) * mat.diffuseAlbedo * light.diffuse * intensity;
    vec3 specular = pow(max(dot(lightReflection, surf.viewDir), 0.0), mat.shininess) * mat.specularAlbedo * light.diffuse * intensity;
    vec3 ambient = (mat.ambient * light.ambient);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    return lighting;
}