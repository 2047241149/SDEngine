#include "BRDF.fx"
#include "ImportanceSampleGGX.fx"
//https://learnopengl.com/PBR/IBL/Specular-IBL

TextureCube HdrCubeMap:register(t0);
SamplerState TrilinearFliterClamp:register(s0);


cbuffer CBMatrix:register(b0)
{
	matrix View;
	matrix Proj;
};

cbuffer CBParam1:register(b1)
{
	float Roughness;
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
	outa.Pos.z = outa.Pos.w;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float3 N = normalize(outa.SkyPos);
	float3 R = N;
	float3 V = R;

	const uint SAMPLE_COUNT = 1024;
	float3 perfilteredColor = float3(0.0, 0.0, 0.0);
	float totalWeight = 0.0;

	for (uint i = 0; i < SAMPLE_COUNT; ++i)
	{
		// generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, N, Roughness);
		float3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0);
		if (NdotL > 0.0)
		{
			float D = DistributionGGX(N, H, Roughness);
			float NdotH = max(dot(N, H), 0.0);
			float HdotV = max(dot(H, V), 0.0);
			float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

			float resolution = 512.0; // resolution of source cubemap (per face)
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
			perfilteredColor += HdrCubeMap.SampleLevel(TrilinearFliterClamp, L, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	perfilteredColor /= totalWeight;
	return float4(perfilteredColor, 1.0);
}