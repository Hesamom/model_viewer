cbuffer globals_vert : register(b0)
{
	float4x4 gWorldViewProj; 
};

cbuffer globals_frag : register(b1)
{
	float4 _color;
};


cbuffer globals_common : register(b2)
{
	float _mul;
};



struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, _mul), gWorldViewProj);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return _color * _mul;
}


