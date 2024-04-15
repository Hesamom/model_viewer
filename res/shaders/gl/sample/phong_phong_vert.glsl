#version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec2 v_uv0;


layout(std140) uniform u_PerFrameGlobals
{
	mat4 m_ViewProjection;
	mat4 m_Projection;
};

uniform mat4 m_Model;
uniform mat4 m_MV;

uniform mat4 m_LightViewProjection;
uniform mat4[4] m_SpotLightViewProjection;
uniform int u_spotLightCount = 0;

out VS_OUT
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	vec4 fragPosLightSpace;
	vec4[4] fragSpotPosLightSpace;
} vs_out;


void main()
{
	//position and normal in view space
	vec4 pos = m_MV * v_position;
	vec3 normal = normalize(mat3(transpose(inverse(m_Model))) * v_normal);

	vs_out.normal = normal;
	vs_out.texCoord = v_uv0;
	vs_out.fragPosLightSpace = m_LightViewProjection * m_Model * v_position;
	vs_out.fragPos = (m_Model * v_position).xyz;

	for (int i = 0; i < u_spotLightCount; i++)
	{
		vs_out.fragSpotPosLightSpace[i] = m_SpotLightViewProjection[i] * m_Model * v_position;
	}
	
	gl_Position = m_Projection * pos;
}