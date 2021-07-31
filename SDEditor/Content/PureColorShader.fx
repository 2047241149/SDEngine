Texture2D BaseTexture:register(t0);
SamplerState SampleWrapLinear:register(s0);

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};

cbuffer CBEveryFrame:register(b1)
{
	float4 surfaceColor;
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
	float2 Tex:TEXCOORD0;
};

VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);

	outa.Tex = ina.Tex;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float4 diffuse = BaseTexture.Sample(SampleWrapLinear, outa.Tex);
	return surfaceColor * diffuse;
}