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
};

VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	return float4(0.15f, 0.15f, 0.65f, 1.0f);
}