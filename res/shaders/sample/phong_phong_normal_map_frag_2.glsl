   #version 330 core

uniform sampler2D u_diffuseSampler;
uniform sampler2D u_normalSampler;
   
uniform vec3 u_light_color;

uniform vec3 u_ambient;
uniform vec3 u_diffuseAlbedo;
uniform vec3 u_specularAlbedo;
uniform float u_shininess;

in VS_OUT
{
    vec2 texCoord;
    vec3 lightPos;
    vec3 fragPos;
} fs_in;

out vec4 FragColor;

void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(u_normalSampler, fs_in.texCoord).rgb;
    // transform normal vector to range [-1,1]
    normal = normal * 2.0 - 1.0;

    vec3 lightDir = normalize(fs_in.lightPos - fs_in.fragPos);
    vec3 viewDir = -normalize(fs_in.fragPos);
    vec3 lightReflection = reflect(-lightDir, normal);

    //calculate light components
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_diffuseAlbedo;
    vec3 specular = pow(max(dot(lightReflection, viewDir), 0.0), u_shininess) * u_specularAlbedo;
    vec3 color = diffuse + specular + (u_ambient * u_light_color);

    vec3 diffuseTextel = texture(u_diffuseSampler, fs_in.texCoord).rgb;
    FragColor = vec4(color * diffuseTextel, 1.0);
}