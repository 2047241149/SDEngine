
#include "BRDF.fx"
#include "ToneMap.fx"

#define GroundThreadSize 16


struct LightGrid
{
	float offset;
	float count;
};

//点光源
struct PointLight
{
	float3 pos;
	float3 color;
	float radius;
	float4 attenuation;
};

cbuffer CBCommon:register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float3 cameraPos;
	float farPlane;
	float nearPlane;
	float lightCount;
	float4 tileSizes;
	float2 cluserFactor;
	float bDebugLightCount;
};

cbuffer CBMatrix:register(b1)
{
	matrix View;
	matrix ProjInv;
};

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};

Texture2D<float4> DepthTex:register(t0);
Texture2D<float4> WorldPosTex:register(t1);
Texture2D<float4> WorldNormalTex:register(t2);
Texture2D<float4> SpecularRoughMetalTex:register(t3);
Texture2D<float4> AlbedoTex:register(t4);
SamplerState clampLinearSample:register(s0);
StructuredBuffer<PointLight> PointLights : register(t5);
RWTexture2D<float4> OutputTexture : register(u0);
RWStructuredBuffer<LightGrid> LightGridList : register(u1);
RWStructuredBuffer<float> GlobalLightIndexList : register(u2);


[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{

	float2 uv = float2(float(dispatchThreadId.x) / ScreenWidth, float(dispatchThreadId.y) / ScreenHeight);
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	float2 screenPos = float2(uv.x * ScreenWidth, uv.y * ScreenHeight);
	float depth = DepthTex.SampleLevel(clampLinearSample, uv, 0).r;
	float viewZ = DepthBufferConvertToLinear(depth);
	uint clusterZ = uint(max(log2(viewZ) * cluserFactor.x + cluserFactor.y, 0.0));
	uint3 clusters = uint3(uint(screenPos.x / tileSizes.w), uint(screenPos.y / tileSizes.w), clusterZ);
	uint clusterIndex = clusters.x + clusters.y * (uint)tileSizes.x + clusters.z * (uint)tileSizes.x * (uint)tileSizes.y;
	uint lightOffset = (uint)LightGridList[clusterIndex].offset;
	uint lightCount = (uint)LightGridList[clusterIndex].count;

	//G-Buffer-Pos(浪费1 float)
	float3 worldPos = WorldPosTex.SampleLevel(clampLinearSample, uv, 0).xyz;

	//G-Buffer-Normal(浪费1 float)
	float3 worldNormal = WorldNormalTex.SampleLevel(clampLinearSample, uv, 0).xyz;
	worldNormal = normalize(worldNormal);

	float3 albedo = AlbedoTex.SampleLevel(clampLinearSample, uv, 0).xyz;

	//G-Buffer-Specual-Rough-Metal(浪费1 float)
	float3 gBufferAttrbite = SpecularRoughMetalTex.SampleLevel(clampLinearSample, uv, 0).xyz;
	float specular = gBufferAttrbite.x;
	float roughness = gBufferAttrbite.y;
	float metal = gBufferAttrbite.z;

	for (uint i = 0; i < lightCount; ++i)
	{
		uint lightIndex = GlobalLightIndexList[lightOffset + i];
		PointLight light = PointLights[lightIndex];
		float3 lightPos = light.pos;
		float3 lightColor = light.color;
		float4 attenuation = light.attenuation;

		float3 pixelToLightDir = lightPos - worldPos;
		float distance = length(pixelToLightDir);
		float3 L = normalize(pixelToLightDir);
		float3 V = normalize(cameraPos - worldPos);
		float3 H = normalize(L + V);
		float attenua = 1.0 / (attenuation.x + attenuation.y * distance + distance * distance * attenuation.z);
		float3 radiance = lightColor * attenua;

		//f(cook_torrance) = D* F * G /(4 * (wo.n) * (wi.n))
		float D = DistributionGGX(worldNormal, H, roughness);
		float G = GeometrySmith(worldNormal, V, L, roughness);
		float3 fo = GetFresnelF0(albedo, metal);
		float cosTheta = max(dot(V, H), 0.0);
		float3 F = FresnelSchlick(cosTheta, fo);
		float3 ks = F;
		float3 kd = float3(1.0, 1.0, 1.0) - ks;
		kd *= 1.0 - metal;

		float3 dfg = D * G * F;
		float nDotl = max(dot(worldNormal, L), 0.0);
		float nDotv = max(dot(worldNormal, V), 0.0);
		float denominator = 4.0 * nDotv * nDotl;
		float3 specularFactor = dfg / max(denominator, 0.001);

		//specular贴图相当于specular遮罩
		color.xyz += (kd * albedo / PI + specularFactor * specular) * radiance * nDotl * 2.2;
	}

	if (bDebugLightCount == 1)
	{
		float value = lightCount / 100.0;

		if (value == 0)
		{
			color = float4(0.0, 0.0, 0.0, 1.0);
		}
		else if (value <= 0.1)
		{
			color = float4(1.0, 0.0, 0.0, 1.0);
		}
		else if (value <= 0.2)
		{
			color = float4(0.0, 1.0, 0.0, 1.0);
		}
		else if (value <= 0.4)
		{
			color = float4(0.0, 0.0, 1.0, 1.0);
		}
		else if (value <= 0.6)
		{
			color = float4(1.0, 1.0, 0.0, 1.0);
		}
		else if (value <= 0.8)
		{
			color = float4(0.0, 1.0, 1.0, 1.0);
		}
		else
		{
			color = float4(1.0, 1.0, 1.0, 1.0);
		}
	}

	OutputTexture[dispatchThreadId.xy] = color;
}