Texture2D DiffuseTex:register(t0);
Texture2D LightBufferTex:register(t1);
SamplerState clampLinearSample:register(s0);  


//ACES: https://zhuanlan.zhihu.com/p/21983679
//GammaCorrect: https://learnopengl.com/Advanced-Lighting/Gamma-Correction
float3 ACESToneMappingAndGammaCorrect(float3 color, float adaptedLum)
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;

	color *= adaptedLum;
	color = (color * (a * color + b)) / (color * (c * color + d) + e);
	float gammaCorrect = 1.0 / 2.2;
	color = pow(color, gammaCorrect);
	return color;
}

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
	float4 light = LightBufferTex.Sample(clampLinearSample, outa.Tex);
	float4 diffuse = DiffuseTex.Sample(clampLinearSample, outa.Tex);
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	color = float4(diffuse.xyz * light.xyz, 1.0);
	color.rgb = ACESToneMappingAndGammaCorrect(color.rgb, 1.0);
	return color;
}