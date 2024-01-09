   #version 330 core
in vec4 v_position;
in vec3 v_normal;
in vec2 v_uv0;
in vec3 v_tangent;
   
   
uniform vec3 u_light_dir;
uniform mat4 m_Model;
uniform mat4 m_MV;
uniform mat4 m_Projection;
uniform mat4 m_LightViewProjection;
   
out VS_OUT
{
    vec2 texCoord;
    vec3 lightDir;
    vec3 viewDir;
    vec4 fragPosLightSpace;
} vs_out;


void main()
{
    vec3 T = normalize(vec3(m_Model * vec4(v_tangent, 0.0)));
    vec3 N = normalize(vec3(m_Model * vec4(v_normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));


    //position and normal in view space
    vec4 pos = m_MV * v_position;
    vec3 viewDir = -pos.xyz;

    vec3 lightDir = TBN * (u_light_dir);
    viewDir = TBN * (viewDir);
    
    vs_out.lightDir = lightDir;
    vs_out.viewDir = viewDir;
    vs_out.texCoord = v_uv0;
    vs_out.fragPosLightSpace = m_LightViewProjection * m_Model * v_position;

    gl_Position = m_Projection * pos;
}