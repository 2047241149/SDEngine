Texture2D ScreenRT:register(t0);
SamplerState ClampLinear:register(s0);

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
	return ScreenRT.Sample(ClampLinear, outa.Tex);
}