cbuffer globals_vert : register(b0)
{
	float4x4 m_MV;
	float4x4 m_Model;
	float4x4 m_Projection;
};


struct VertexIn
{
	float3 Pos  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.Pos = mul(float4(vin.Pos, 1.0f), m_MV);
	vout.Pos = mul(vout.Pos,  m_Projection);
	vout.Color = vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}