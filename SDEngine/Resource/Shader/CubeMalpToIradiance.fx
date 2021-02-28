#include "ToneMap.fx"

//https://learnopengl.com/PBR/IBL/Diffuse-irradiance

TextureCube HdrCubeMap:register(t0);
SamplerState ClampLinear:register(s0);

static const float PI = 3.1415926;

cbuffer CBMatrix:register(b0)
{
	matrix View;
	matrix Proj;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float3 Color:COLOR;
	float3 Normal:NORMAL;
	float3 Tangent:TANGENT;
	float2 Tex:TEXCOORD;
};


struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 SkyPos:TEXCOORD0;
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.SkyPos = ina.Pos;
	outa.Pos = float4(ina.Pos, 1.0f);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float3 N = normalize(outa.SkyPos);
	float3 irradiance = float3(0.0, 0.0, 0.0);
	float3 up = float3(0.0, 1.0, 0.0);
	float3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			
			irradiance += HdrCubeMap.Sample(ClampLinear, sampleVec).xyz * cos(theta) * sin(theta);
			nrSamples += 1.0;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	return float4(irradiance, 1.0);
}