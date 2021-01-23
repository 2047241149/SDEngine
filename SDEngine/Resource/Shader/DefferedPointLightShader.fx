
#include "BRDF.fx"

Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SpecularRoughMetalTex:register(t2);
Texture2D AlbedoTex:register(t3);
SamplerState clampLinearSample:register(s0);  


cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	float3 cameraPos;
};


cbuffer CBPointLight:register(b1)
{
	float4 lightColor;
	float3 lightPos;
	float4 attenuation;
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
	float4 HCPos:TEXCOORD0;
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.HCPos = outa.Pos;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float2 uv = (outa.HCPos.xy / outa.HCPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
	float4 color = float4(0.0, 0.0, 0.0, 1.0);

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

	float3 pixelToLightDir = lightPos - worldPos;
	float distance = length(pixelToLightDir);
	float3 L = normalize(pixelToLightDir);
	float3 V = normalize(cameraPos - worldPos);
	float3 H = normalize(L + V);
	float attenua = 1.0 / (attenuation.x + attenuation.y * distance + distance * distance * attenuation.z);
	float3 radiance = lightColor.xyz * attenua;

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
	color.xyz = (kd * albedo / PI + specularFactor * specular) * radiance * nDotl;
	return color;
}