
#include "BRDF.fx"

struct LightGrid
{
	float offset;
	float count;
};

struct PointLight
{
	float3 pos;
	float3 color;
	float radius;
	float4 attenuation;
};

Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SpecularRoughMetalTex:register(t2);
Texture2D AlbedoTex:register(t3);
Texture2D DepthTex:register(t4);
SamplerState clampLinearSample:register(s0);
RWStructuredBuffer<LightGrid> LightGridList : register(u1);
RWStructuredBuffer<float> GlobalLightIndexList : register(u2);
RWStructuredBuffer<PointLight> PointLightList : register(u3);

cbuffer CBCommon:register(b0)
{
	float3 cameraPos;
	float4 tileSizes;
	float ScreenWidth;
	float ScreenHeight;
	float2 cluserFactor;
	float farPlane;
	float nearPlane;
};

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

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};

float4 PS(VertexOut outa) : SV_Target
{
	float2 uv = outa.Tex;
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	float2 screenPos = float2(uv.x * ScreenWidth, uv.y * ScreenHeight);
	float depth = DepthTex.Sample(clampLinearSample, outa.Tex).r;
	float viewZ = DepthBufferConvertToLinear(depth);
	uint clusterZ = uint(max(log2(viewZ) * cluserFactor.x + cluserFactor.y, 0.0));
	uint3 clusters = uint3(uint(screenPos.x / tileSizes.w), uint(screenPos.y / tileSizes.w), clusterZ);
	uint clusterIndex = clusters.x + clusters.y * (uint)tileSizes.x + clusters.z * (uint)tileSizes.x * (uint)tileSizes.y;
	uint lightOffset = (uint)LightGridList[clusterIndex].offset;
	uint lightCount = (uint)LightGridList[clusterIndex].count;

	//G-Buffer-Pos(浪费1 float)
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, uv).xyz;

	//G-Buffer-Normal(浪费1 float)
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, uv).xyz;
	worldNormal = normalize(worldNormal);

	float3 albedo = AlbedoTex.Sample(clampLinearSample, uv).xyz;

	//G-Buffer-Specual-Rough-Metal(浪费1 float)
	float3 gBufferAttrbite = SpecularRoughMetalTex.Sample(clampLinearSample, uv).xyz;
	float specular = gBufferAttrbite.x;
	float roughness = gBufferAttrbite.y;
	float metal = gBufferAttrbite.z;

	for (uint i = 0; i < lightCount; ++i)
	{
		uint lightIndex = GlobalLightIndexList[lightOffset + i];
		PointLight light = PointLightList[lightIndex];
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

	return color;
}