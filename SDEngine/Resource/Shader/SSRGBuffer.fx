Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
SamplerState SampleWrapLinear:register(s0);
SamplerState SampleClampPoint:register(s1);

cbuffer CBMatrix:register(b0)
{
	matrix View;
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
	outa.Pos = float4(ina.Pos.xy, 0.0, 1.0);
	outa.Tex = ina.Tex;
	return outa;
}


struct PixelOut
{
	float3 ViewPos:SV_Target1;
	float3 ViewNormal:SV_Target2;
};


PixelOut PS(VertexOut outa) : SV_Target
{
	PixelOut pixelOut;
	float3 worldPos = WorldPosTex.Sample(SampleClampPoint, outa.Tex).xyz;
	float3 worldNormal = WorldNormalTex.Sample(SampleClampPoint, outa.Tex).xyz;
	pixelOut.ViewPos = mul(float4(worldPos, 1.0), View).xyz;
	pixelOut.ViewNormal = mul(worldNormal, (float3x3)View).xyz;
	return pixelOut;
}