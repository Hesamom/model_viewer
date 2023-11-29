#version 330 core

uniform sampler2D m_TextureSampler;
uniform vec3 u_ambient = vec3(0.1, 0.1, 0.1);
uniform vec3 u_diffuseAlbedo = vec3(0.5, 0.5, 0.5);
uniform vec3 u_specularAlbedo = vec3(0.5, 0.5, 0.5);
uniform float u_shininess = 128.0;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
} fs_in;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(fs_in.lightDir);
    vec3 viewDir = normalize(fs_in.viewDir);

    vec3 lightReflection = reflect(-lightDir, normal);

    //calculate light components 
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_diffuseAlbedo;
    vec3 specular = pow(max(dot(lightReflection, viewDir), 0.0), u_shininess) * u_specularAlbedo;
    vec3 color = diffuse + specular + u_ambient;
    
    vec3 texel = texture(m_TextureSampler, fs_in.texCoord).rgb;
    FragColor = vec4(color, 1.0);
}