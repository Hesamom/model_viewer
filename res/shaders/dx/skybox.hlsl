TextureCube u_skybox : register(t0);

SamplerState samPointWrap : register(s0); 
SamplerState samPointClamp : register(s1); 
SamplerState samLinearWrap : register(s2);
SamplerState samLinearClamp : register(s3); 
SamplerState samAnisotropicWrap : register(s4); 
SamplerState samAnisotropicClamp : register(s5);


cbuffer globals_vert : register(b0)
{
	float4x4 m_MV;
    float4x4 m_Projection;
};


struct VertexIn
{
	float3 Pos  : POSITION;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
	float3 UV : TEXTCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.Pos = mul(float4(vin.Pos, 1.0f),  m_MV);
	vout.Pos = mul(vout.Pos,  m_Projection);
	vout.UV = vin.Pos.xyz;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return u_skybox.Sample(samPointWrap, pin.UV);
	//return float4(0,1,0,1);
}


