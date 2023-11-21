#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_normal;

uniform mat4 m_MVP;

void main()
{
	//const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0), vec4(-0.25, -0.25, 0.5, 1.0), vec4( 0.25, 0.25, 0.5, 1.0));
	//gl_Position = vertices[gl_VertexID % 3];
	gl_Position = vec4(0.0, 0.0, 0.5, 1.0);
	//gl_Position =  v_position;
}