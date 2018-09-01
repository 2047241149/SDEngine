Texture2D ScreenRT:register(t0);
SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);
SamplerState anisotropicSample:register(s2);


cbuffer CBFxaa:register(b0)
{
	float4 rcpFrame;
}

float2 texSize(Texture2D tex)
{
	uint texWidth, texHeight;
	tex.GetDimensions(texWidth, texHeight);
	return float2(texWidth, texHeight);
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


struct FxaaTex
{
	SamplerState smp;
	Texture2D tex;
};


float4 FxaaTexLod0(FxaaTex tex, float2 uv)
{
	return tex.tex.SampleLevel(tex.smp, uv.xy, 0);
}


float4 FxaaTexGrad(FxaaTex tex, float2 uv, float2 grad)
{

}


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = float4(ina.Pos.xy,1.0,1.0);
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	
	return color;
}