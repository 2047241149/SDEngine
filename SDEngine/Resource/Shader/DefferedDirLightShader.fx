#include "BRDF.fx"

//TODO: 多PointLight，多DirLight. IrradianceMap共同作为一个Shader文件解决
Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SpecularRoughMetalTex:register(t2);
Texture2D DirLightShadowMap:register(t3);
Texture2D SSAORT:register(t4);
Texture2D AlbedoTex:register(t5);
TextureCube IrradianceTex:register(t6);
TextureCube PrefliterCubeMap:register(t7);
Texture2D BrdfLut:register(t8);
Texture2D LightBuffer:register(t9);

SamplerState clampLinearSample:register(s0);  
SamplerState TrilinearFliterClamp:register(s1);

cbuffer CBMatrix:register(b0)
{
	float3 cameraPos;
};

cbuffer CBDirLight:register(b1)
{
	float4 lightColor;  //w存在的为强度
	float3 lightDir;
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

float4 PS(VertexOut outa) : SV_Target
{
	float4 color = float4(0.0, 0.0, 0.0, 1.0);

	//GBuffer
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, outa.Tex).xyz;
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, outa.Tex).xyz;
	worldNormal = normalize(worldNormal);
	float3 gBufferAttrbite = SpecularRoughMetalTex.Sample(clampLinearSample, outa.Tex).xyz;
	float specular = gBufferAttrbite.x;
	float roughness = gBufferAttrbite.y;
	float metal = gBufferAttrbite.z;
	float ao = SSAORT.Sample(clampLinearSample, outa.Tex).r;
	float3 albedo = AlbedoTex.Sample(clampLinearSample, outa.Tex).xyz;
	float3 pointLightColor = LightBuffer.Sample(clampLinearSample, outa.Tex).xyz;

	//light
	float3 L = -normalize(lightDir);
	float3 V = normalize(cameraPos - worldPos);
	float3 H = normalize(L + V);
	float3 radiance = lightColor.xyz;
	float3 R = reflect(-V, worldNormal);

	//f(cook_torrance) = D* F * G /(4 * (wo.n) * (wi.n))
	float D = DistributionGGX(worldNormal, H, roughness);
	float G = GeometrySmith(worldNormal, V, L, roughness);
	float3 fo = GetFresnelF0(albedo, metal);
	float cosTheta = max(dot(V, H), 0.0);
	float3 F = FresnelSchlick(cosTheta, fo);
	float3 ks = F;
	float3 kd = 1.0 - ks;
	kd = kd * (1.0 - metal);

	float3 dfg = D * G * F;
	float nDotl = max(dot(worldNormal, L), 0.0);
	float nDotv = max(dot(worldNormal, V), 0.0);
	float denominator = 4.0 * nDotv * nDotl;
	float3 specularFactor = dfg / max(denominator, 0.001);
	float3 irradiance = IrradianceTex.Sample(clampLinearSample, worldNormal).xyz;

	//利用DirLightShadowMap
	const float MAX_REF_LOD = 4.0;
	float3 prefliterColor = PrefliterCubeMap.SampleLevel(TrilinearFliterClamp, R, MAX_REF_LOD * roughness).rgb;
	float2 brdf = BrdfLut.Sample(clampLinearSample, float2(nDotv, roughness)).xy;
	float3 iblSpecular = prefliterColor * (ks * brdf.x + brdf.y);
	float3 iblDiffuse = irradiance * albedo * kd / PI;
	float3 shadowFactor = DirLightShadowMap.Sample(clampLinearSample, outa.Tex).rgb;
	float3 dirLightColor = (kd * albedo / PI + specularFactor * specular) * radiance * nDotl * shadowFactor;
	float3 iblColor = (iblDiffuse + iblSpecular) * ao;
	color.xyz = dirLightColor + iblColor + pointLightColor;
	color.w = 1.0;
	return color;
}