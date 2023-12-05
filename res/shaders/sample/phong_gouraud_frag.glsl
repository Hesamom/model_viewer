#version 330 core

uniform sampler2D u_diffuseSampler;

in vec3 f_color;
in vec2 f_texCoord;

out vec4 FragColor;

void main()
{
    vec3 texel = texture(u_diffuseSampler, f_texCoord).rgb;
    FragColor = vec4(f_color * texel, 1.0);
}