cbuffer globals_vert : register(b0)
{
	float4x4 gWorldViewProj; 
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 UV0 : TEXTCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 UV0 : TEXTCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.TEXTCOORD = vin.TEXTCOORD;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
//TODO map from the texture 
    return * _mul;
}


