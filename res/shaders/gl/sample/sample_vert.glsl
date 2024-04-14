#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_normal;

uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;

layout(std140) uniform myBlock
{
	float varFloat;
	int varInt;
	bool varBool;
	mat4 varMat4;
};

void main()
{
	gl_Position = m_Projection * m_MV * v_position * varFloat;
}