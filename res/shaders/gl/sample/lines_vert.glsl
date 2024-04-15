#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

layout(std140) uniform u_PerFrameGlobals
{
	mat4 m_ViewProjection;
	mat4 m_Projection;
};

uniform mat4 m_MV;

out vec4 f_color;

void main()
{
	gl_Position = m_Projection * m_MV * v_position;
	f_color = v_color;
}