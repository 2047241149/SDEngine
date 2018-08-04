Texture2D ScreenRT:register(t0);
Texture2D ScreenBlurRT:register(t1);
Texture2D DepthRT:register(t2);
SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);


cbuffer CBDOF:register(b0)
{
	float depthStart;
	float depthRange;
	float farPlane;
	float nearPlane;
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

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 /(nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = float4(ina.Pos.xy,1.0,1.0);
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float4 color = ScreenRT.Sample(clampLinearSample, outa.Tex);
	float4 blurColor = ScreenBlurRT.Sample(clampLinearSample, outa.Tex);
	float depth = DepthRT.Sample(clampLinearSample, outa.Tex).r;
	float viewSpaceZ = DepthBufferConvertToLinear(depth);

	if (depth < 1.0f)
	{
		float percent = saturate((viewSpaceZ - depthStart) / depthRange);
		color = lerp(color, blurColor, percent);
	}
	color.a = 1.0;
	return color;
}