TextureCube CubeMap:register(t0);
SamplerState ClampLinear:register(s0);

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float3 Color:COLOR;
	float3 Normal:NORMAL;
	float3 Tangent:TANGENT;
	float2 Tex:TEXCOORD;
};


struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 SkyPos:TEXCOORD0;
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.SkyPos = ina.Pos;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.Pos.z = outa.Pos.w;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	return CubeMap.Sample(ClampLinear, 	outa.SkyPos);
}