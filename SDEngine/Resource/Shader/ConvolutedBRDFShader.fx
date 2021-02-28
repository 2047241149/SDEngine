
#include "ImportanceSampleGGX.fx"

float GeometrySchlickGGXForIBL(float NdotV, float roughness)
{
	// note that we use a different k for IBL
	float a = roughness;
	float k = (a * a) / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmithForIBL(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGXForIBL(NdotV, roughness);
	float ggx1 = GeometrySchlickGGXForIBL(NdotL, roughness);

	return ggx1 * ggx2;
}

float2 IntegrateBRDF(float NDotV, float roughness)
{
	float3 V;
	V.x = sqrt(1.0 - NDotV * NDotV);
	V.y = 0.0;
	V.z = NDotV;

	float A = 0.0;
	float B = 0.0;

	float3 N = float3(0.0, 0.0, 1.0);
	const uint SAMPLE_COUNT = 1024;

	for (uint i = 0; i < SAMPLE_COUNT; ++i)
	{
		// generates a sample vector that's biased towards the
		// preferred alignment direction (importance sampling).
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, N, roughness);
		float3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(L.z, 0.0);
		float NdotH = max(H.z, 0.0);
		float VdotH = max(dot(V, H), 0.0);

		if (NdotL > 0)
		{
			float G = GeometrySmithForIBL(N, V, L, roughness);
			float G_Vis = (G * VdotH) / (NdotH * NDotV);

			//V = Wo
			float FC = pow(1.0 - VdotH, 5.0);

			A += (1.0 - FC) * G_Vis;
			B += FC * G_Vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);

	return float2(A, B);
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
	float2 color2 =  IntegrateBRDF(outa.Tex.x, outa.Tex.y);
	return float4(color2, 0.0, 0.0);
}