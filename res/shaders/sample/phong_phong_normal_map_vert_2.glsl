   #version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec2 v_uv0;
in vec3 v_tangent;

uniform vec3 u_light_pos;

uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;

out VS_OUT
{
    vec2 texCoord;
    vec3 lightPos;
    vec3 fragPos;

} vs_out;
   
 
mat3 getTBN()
{
    vec3 T = normalize(vec3(m_MV * vec4(v_tangent, 0.0)));
    vec3 N = normalize(vec3(m_MV * vec4(v_normal, 0.0)));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    return TBN;
}
   
void main()
{
    mat3 TBN_inverse = transpose(getTBN());
    
    
    //take light and frag positions to tangant space to avoid matrix calculation in frag shader 
    vs_out.lightPos = TBN_inverse * u_light_pos;
    vs_out.fragPos = TBN_inverse * vec3(m_MV * v_position);
    vs_out.texCoord = v_uv0;

    
    gl_Position = m_Projection * m_MV * v_position;
}