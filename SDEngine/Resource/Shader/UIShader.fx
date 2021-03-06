#include "ToneMap.fx"

Texture2D ShaderTexture:register(t0);
SamplerState LinearClamp:register(s0);  


cbuffer CBMatrix:register(b0)
{
	matrix UIView;
	matrix UIOrtho;
};

struct VertexIn
{
	float3 Pos:POSITION;
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
	outa.Pos = mul(float4(ina.Pos, 1.0f), UIView);
	outa.Pos = mul(outa.Pos, UIOrtho);

	outa.Tex= ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float4 color = ShaderTexture.Sample(LinearClamp, outa.Tex);
	color = float4(color.xyz, 1.0);
	color.rgb = ACESToneMappingAndGammaCorrect(color.rgb, 1.0);
	return color;
}