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
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.PosL, _mul), gWorldViewProj);
	vout.Color = vin.Color;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color * _color * _mul;
}


