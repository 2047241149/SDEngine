Texture2D ScreenRT:register(t0);
SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);


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
	float2 diffuseTexSize = texSize(ScreenRT);
	diffuseTexSize = 1.0 / diffuseTexSize;
	float3 color = float3(0.0, 0.0, 0.0);
	//权重和为1.0
	float avg[3][3] = 
	{ 
		0.1,0.1,0.1,
		0.1,0.2,0.1,
		0.1,0.1,0.1
	};
	float avgSum = 0.0;
	for (int a = 0; a <  3; ++a)
	{
		for (int b = 0; b <  3; ++b)
		{
			avgSum = avgSum + avg[a][b];
		}
	}
	
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			float2 texcoord = outa.Tex + i * diffuseTexSize.x + j * diffuseTexSize.y;
			color = color + (avg[i+ 1][j + 1] / avgSum) * ScreenRT.Sample(clampLinearSample, texcoord).xyz;
		}
	}
	
	return float4(color, 1.0);
}