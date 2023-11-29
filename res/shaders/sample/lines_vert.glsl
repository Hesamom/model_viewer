#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

uniform mat4 m_MV;
uniform mat4 m_Projection;

out vec4 f_color;

void main()
{
	gl_Position = m_Projection * m_MV * v_position;
	f_color = v_color;
}