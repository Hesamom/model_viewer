#version 330 core
in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D m_TextureSampler;

out vec4 FragColor;

void main()
{
    vec3 color = texture(m_TextureSampler, TexCoord).rgb;
    FragColor = vec4(color, 1.0);
}