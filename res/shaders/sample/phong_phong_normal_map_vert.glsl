   #version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec2 v_uv0;
in vec3 v_tangent;
in vec3 v_bitangent;

uniform vec3 u_light_pos;

uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    mat3 TBN;
} vs_out;


void main()
{
    vec3 T = normalize(vec3(m_Model * vec4(v_tangent,   0.0)));
    vec3 B = normalize(vec3(m_Model * vec4(v_bitangent, 0.0)));
    vec3 N = normalize(vec3(m_Model * vec4(v_normal,    0.0)));
    mat3 TBN = mat3(T, B, N);

    //position and normal in view space
    vec4 pos = m_MV * v_position;
    vec3 normal = mat3(m_MV) * v_normal;
    vec3 lightDir = u_light_pos - pos.xyz;
    vec3 viewDir = -pos.xyz;

    vs_out.normal = normal;
    vs_out.lightDir = lightDir;
    vs_out.viewDir = viewDir;
    vs_out.texCoord = v_uv0;
    vs_out.TBN = TBN;


    gl_Position = m_Projection * pos;
}