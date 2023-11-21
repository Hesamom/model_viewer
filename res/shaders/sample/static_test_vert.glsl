#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_normal;

out vec4 u_color;
uniform mat4 m_MVP;

void main()
{
	gl_Position = m_MVP * v_position;
	u_color = v_position;
}