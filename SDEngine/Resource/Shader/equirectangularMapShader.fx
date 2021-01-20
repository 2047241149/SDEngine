Texture2D EquirectangularMap:register(t0);
SamplerState ClampLinear:register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);

/*
https://learnopengl.com/PBR/IBL/Diffuse-irradiance
*/
float2 SampleSphericalMap(float3 v)
{
	float2 uv = float2(atan2(v.x, v.z), asin(v.y));
	uv *= invAtan;
	uv.y = 1.0 - uv.y;
	uv += 0.5;
	return uv;
}

cbuffer CBMatrix:register(b0)
{
	matrix View;
	matrix Proj;
};

struct VertexIn
{
	float3 Pos:POSITION;
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 LocalPos:TEXCOORD0;
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.LocalPos = ina.Pos;
	outa.Pos = mul(float4(ina.Pos, 1.0f), View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.Pos.z = outa.Pos.w;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float2 uv = SampleSphericalMap(outa.LocalPos);
	float3 color = EquirectangularMap.Sample(ClampLinear, uv).rgb;
	return float4(color, 1.0);
}