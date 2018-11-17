Texture2D ScreenRT:register(t0);
SamplerState ClampLinear:register(s0);

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



VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = float4(ina.Pos.xy,1.0,1.0);
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float2 texelSize = 1.0 / texSize(ScreenRT);
	float result = 0.0;

	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			result += ScreenRT.Sample(ClampLinear, outa.Tex + float2(x, y) * texelSize).r;
		}
	}

	result /= (4.0 * 4.0);

	return float4(result, result, result, 1.0);
}