SamplerState samPointWrap : register(s0); 
SamplerState samPointClamp : register(s1); 
SamplerState samLinearWrap : register(s2);
SamplerState samLinearClamp : register(s3); 
SamplerState samAnisotropicWrap : register(s4); 
SamplerState samAnisotropicClamp : register(s5);


struct material
{
	float3 ambient;
	float3 diffuseAlbedo;
	float3 specularAlbedo;

	float shininess;
	float opacity;
	float reflectivity;
};

struct surface
{
	float3 viewDir;
	float3 normal;
	float3 fragPos;
    
	float3 specTexel;
	float3 diffuseTexel;
};

struct directLight
{
	float3 direction;
	float3 ambient;
	float3 diffuse;
};

struct baseLight
{
	float3 ambient;
	float3 diffuse;
	float3 toSourceDirection;
	float intensity;
};

cbuffer u_globals : register(b0)
{
	float4x4 m_MV;
	float4x4 m_Model;
	float4x4 m_Projection;
	
	float3 u_lightAmbient;
	float3 u_lightDiffuse;
	float3 u_light_dir;
	
	float3 u_camera_pos;
};

cbuffer u_mat : register(b1)
{
	material mat;
};


Texture2D diffuseSampler : register(t0);
Texture2D specularSampler : register(t1);
Texture2D normalSampler : register(t2);

struct VertexIn
{
	float3 Pos  : POSITION;
	float2 UV0 : TEXTCOORD;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV0 : TEXTCOORD0;
	float3 FragPos : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.Pos = mul(float4(vin.Pos, 1.0f), m_MV);
	vout.Pos = mul(vout.Pos, m_Projection);

	vout.FragPos = mul(float4(vin.Pos, 1.0f), m_Model).xyz;

	vout.Normal = mul(vin.Normal, (float3x3)m_Model);
	vout.UV0 = vin.UV0;

	return vout;
}


float getSpecular(surface surf, baseLight light)
{
#ifdef SPEC_BLIN
	float3 halfVec = normalize(surf.viewDir + light.toSourceDirection);
	float spec = max(dot(surf.normal, halfVec), 0.0);
	return spec;
#else
	float3 lightReflection = reflect(-light.toSourceDirection, surf.normal);
	float spec = max(dot(lightReflection, surf.viewDir), 0.0);
	return spec;
#endif
}

float3 computeLight(surface surf, float shadow, material mat, baseLight light)
{
	float3 diffuse = max(dot(surf.normal, light.toSourceDirection), 0.0) * mat.diffuseAlbedo * light.diffuse * light.intensity * surf.diffuseTexel;
    
	float spec = getSpecular(surf, light);
	float3 specular = pow(spec, mat.shininess) * mat.specularAlbedo * light.diffuse * light.intensity * surf.specTexel;
	float3 ambient = mat.ambient * light.ambient * light.intensity * surf.diffuseTexel;

	float3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
	return lighting;
}


float3 getDirLight(surface surf, directLight directlight, float shadow, material mat)
{
	baseLight light;
	light.intensity = 1;
	light.toSourceDirection = directlight.direction;
	light.ambient = directlight.ambient;
	light.diffuse = directlight.diffuse;

	return computeLight(surf, shadow, mat, light);
}

float4 PS(VertexOut pin) : SV_Target
{
	surface surf;
	surf.fragPos = pin.FragPos;
	surf.viewDir = normalize(u_camera_pos - pin.FragPos);
	surf.normal = normalize(pin.Normal);
	surf.specTexel = specularSampler.Sample(samPointWrap, pin.UV0).xyz;
	surf.diffuseTexel = diffuseSampler.Sample(samPointWrap, pin.UV0).xyz;
    
	directLight dirLight;
	dirLight.direction = normalize(u_light_dir);
	dirLight.ambient = u_lightAmbient;
	dirLight.diffuse = u_lightDiffuse;

	float3 finalColor =  getDirLight(surf, dirLight, 0, mat);
	return float4(finalColor,1);
}