Texture2D DiffuseTex:register(t0);
Texture2D LightBufferTex:register(t1);
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
	float4 light = LightBufferTex.Sample(clampLinearSample, outa.Tex);
	float4 diffuse = DiffuseTex.Sample(clampLinearSample, outa.Tex);
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	color = float4(diffuse.xyz * light.xyz, 1.0);
	float gammaCorrect = 1.0 / 2.2;
	color = pow(color, float4(gammaCorrect, gammaCorrect, gammaCorrect, 1.0));
	return color;
}