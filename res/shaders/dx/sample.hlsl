cbuffer u_globals : register(b0)
{
	float4x4 m_MV;
	float4x4 m_Model;
};

cbuffer u_PerFrameGlobals : register(b1)
{
	float4x4 m_ViewProjection;
	float4x4 m_Projection;
};


struct VertexIn
{
	float3 Pos  : POSITION;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.Pos = mul(float4(vin.Pos, 1.0f), m_MV);
	vout.Pos = mul(vout.Pos, m_Projection);
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(1,0,1,1);
}