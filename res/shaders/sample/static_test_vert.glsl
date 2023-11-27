#version 330 core
layout (location = 0) in vec4 v_position;
layout (location = 1) in vec2 v_uv0;
layout (location = 2) in vec3 v_normal;

uniform mat4 m_MVP;


out vec2 TexCoord;
out vec3 Normal;

void main()
{
	gl_Position = m_MVP * v_position;
	TexCoord = v_uv0;
	Normal = v_normal;
}