#version 330 core
in vec4 v_position;

out vec3 TexCoord;

layout(std140) uniform u_PerFrameGlobals
{
    mat4 m_ViewProjection;
    mat4 m_Projection;
};

uniform mat4 m_MV;

void main()
{
    TexCoord =  v_position.xyz;
    gl_Position = m_Projection * mat4(mat3(m_MV)) * v_position;
}