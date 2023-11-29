#version 330 core
in vec2 f_texCoord;
in vec3 f_normal;

uniform vec3 u_ambientColor = vec3(0.1, 0.1, 0.1);
uniform vec3 u_diffuseColor = vec3(1, 1, 1);
uniform vec3 u_specularColor;
uniform float u_shininess = 128.0;

uniform vec3 u_diffuseLight = vec3(0.1, 0.1, 0.1);
uniform vec3 u_specularLight = vec3(1, 1, 1);

uniform sampler2D m_TextureSampler;

out vec4 FragColor;

void main()
{
    vec3 color = texture(m_TextureSampler, TexCoord).rgb;
    FragColor = vec4(color, 1.0);
}