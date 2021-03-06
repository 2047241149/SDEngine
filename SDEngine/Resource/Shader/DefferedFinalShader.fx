#include "ToneMap.fx"

Texture2D DiffuseTex:register(t0);
SamplerState clampLinearSample:register(s0);  



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
	outa.Pos = float4(ina.Pos.xy, 1.0, 1.0);
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{	
	float4 color = DiffuseTex.Sample(clampLinearSample, outa.Tex);
	color = float4(color.xyz, 1.0);
	color.rgb = ACESToneMappingAndGammaCorrect(color.rgb, 1.0);
	return color;
}