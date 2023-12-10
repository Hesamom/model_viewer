#version 330 core

in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube u_skybox;

void main()
{
    FragColor = texture(u_skybox, TexCoord);
}