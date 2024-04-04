
TextureCube cubeTexture : register(t0);

SamplerState samPointWrap : register(s0); 
SamplerState samPointClamp : register(s1); 
SamplerState samLinearWrap : register(s2);
SamplerState samLinearClamp : register(s3); 
SamplerState samAnisotropicWrap : register(s4); 
SamplerState samAnisotropicClamp : register(s5);


cbuffer globals_vert : register(b0)
{
	float4x4 gWorldViewProj; 
};


struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 UV0 : TEXTCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.UV0 = vin.PosL.xyz;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return cubeTexture.Sample(samPointWrap, pin.UV0);
}


