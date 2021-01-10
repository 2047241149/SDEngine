/*
the pbr effect is cook torrance
f(cook_torrance) = D* F * G /(4 * (wo.n) * (wi.n))
https://learnopengl.com/PBR/Theory
*/

//NormalDistribution:Trowbridge-Reitz GGX
float DistributionGGX(float3 n, float3 h, float roughness)
{
	float PI = 3.1415926;
	float a2 = roughness * roughness;
	float nDoth = max(dot(n, h), 0.0);
	float nDoth2 = nDoth * nDoth;

	float denom = (nDoth2 * (a2 - 1.0) + 1.0);
	denom = max(PI * denom * denom, 0.001);

	return a2 / denom;
}

//GeometryDistribution: Schlick-GGX
float GeometrySchlickGGX(float nDotv, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	float denom = nDotv * (1.0 - k) + k;
	return nDotv / denom;
}

//GeometrySmith = geometry obstruction(view_direction_obs) + geometry shadowing(light_dir_obs)
float GeometrySmith(float3 normal, float3 vieDir, float3 lightDir, float roughness)
{
	float nDotv = max(dot(normal, vieDir), 0.0);
	float nDotl = max(dot(normal, lightDir), 0.0);
	float ggx1 = GeometrySchlickGGX(nDotv, roughness);
	float ggx2 = GeometrySchlickGGX(nDotl, roughness);
	return ggx1 * ggx2;
}

float3 GetFresnelF0(float3 albedo, float metallic)
{
	float3 f0 = float3(0.04, 0.04, 0.04);
	f0 = lerp(f0, albedo, metallic);
	return f0;
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
	return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}