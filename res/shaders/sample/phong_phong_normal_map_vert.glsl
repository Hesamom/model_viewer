   #version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec2 v_uv0;

   

uniform vec3 u_light_dir;
uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;
uniform mat4 m_LightViewProjection;
uniform mat4[4] m_SpotLightViewProjection;
uniform int u_spotLightCount = 0;
   
out VS_OUT
{
    vec2 texCoord;
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec4[4] fragSpotPosLightSpace;
    mat3 TBN;

} vs_out;

mat3 getTBN(vec3 normal, vec3 tangant, mat4 model)
{
    vec3 T = normalize(vec3(model * vec4(tangant, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));
    return TBN;
}
   
void main()
{
    mat3 TBN = getTBN(v_normal, v_tangent, m_Model);
    vs_out.TBN = TBN;
    vs_out.texCoord = v_uv0;
    vs_out.fragPosLightSpace = m_LightViewProjection * m_Model * v_position;
    vs_out.fragPos = (m_Model * v_position).xyz;

    for (int i = 0; i < u_spotLightCount; i++)
    {
        //not sure about this 
        vs_out.fragSpotPosLightSpace[i] = m_SpotLightViewProjection[i] * m_Model * v_position;
    }
    
    gl_Position = m_Projection * m_MV * v_position;
}