#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

uniform mat4 m_MVP;

out vec4 f_color;

void main()
{
	gl_Position = m_MVP * v_position;
	f_color = v_color;
}