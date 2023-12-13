#version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec2 v_uv0;

uniform vec3 u_light_pos;

uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;
uniform mat4 m_LightViewProjection;

out VS_OUT
{
	vec2 texCoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	vec4 fragPosLightSpace;
} vs_out;


void main()
{
	//position and normal in view space
	vec4 pos = m_MV * v_position;
	vec3 normal = normalize(mat3(m_MV) * v_normal);
	vec3 lightDir = u_light_pos - pos.xyz;
	vec3 viewDir = -pos.xyz;

	vs_out.normal = normal;
	vs_out.lightDir = lightDir;
	vs_out.viewDir = viewDir;
	vs_out.texCoord = v_uv0;
	vs_out.fragPosLightSpace = m_LightViewProjection * m_Model * v_position;
	
	gl_Position = m_Projection * pos;
}