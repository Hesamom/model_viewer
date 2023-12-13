
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
    vec3 lightDir;
    vec3 viewDir;
    vec3 normal;
};

vec3 getLight(surface surf, vec3 lightColor, float shadow, material mat)
{
    vec3 lightReflection = reflect(-surf.lightDir, surf.normal);

    //calculate light components
    vec3 diffuse = max(dot(surf.normal, surf.lightDir), 0.0) * mat.diffuseAlbedo;
    vec3 specular = pow(max(dot(lightReflection, surf.viewDir), 0.0), mat.shininess) * mat.specularAlbedo;
    vec3 ambient = (mat.ambient * lightColor);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    return lighting;
}