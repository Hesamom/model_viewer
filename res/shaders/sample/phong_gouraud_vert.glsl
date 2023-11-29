#version 330 core
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv0;

uniform vec3 u_ambient = vec3(0.1, 0.1, 0.1);
uniform vec3 u_diffuseAlbedo = vec3(1, 1, 1);
uniform vec3 u_specularAlbedo = vec3(1, 1, 1);
uniform float u_shininess = 128.0;

uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
uniform vec3 u_diffuseLight = vec3(1, 1, 1);
uniform vec3 u_specularLight = vec3(1, 1, 1);

uniform mat4 m_MV;
uniform mat4 m_Projection;

out vec3 f_color;
out vec2 f_texCoord;

void main()
{
	//position and normal in view space
	vec4 pos = m_MV * v_position;
	vec3 normal = mat3(m_MV) * v_normal;
	
	vec3 lightDir = light_pos - pos.xyz;
	vec3 viewDir = -pos.xyz;
	
	//normalize
	normal = normalize(normal);
	lightDir = normalize(lightDir);
	viewDir = normalize(viewDir);

	vec3 lightReflection = reflect(-lightDir, normal);

	//calculate light components 
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * u_diffuseAlbedo; 
	vec3 specular = pow(max(dot(lightReflection, viewDir), 0.0), u_shininess) * u_specularAlbedo;
	f_color = u_ambient + diffuse + specular;
	f_texCoord = v_uv0;
	gl_Position = m_Projection * pos;
}