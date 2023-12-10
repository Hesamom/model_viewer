#version 330 core
in vec4 v_position;

out vec3 TexCoord;

uniform mat4 m_MV;
uniform mat4 m_Projection;

void main()
{
    TexCoord =  v_position.xyz;
    gl_Position = m_Projection * m_MV * v_position;
}