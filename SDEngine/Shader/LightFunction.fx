

//求平行光因子
float GetDirLightFactor(float3 _dirLightDir, float3 worldNormal)
{
	float lightFactor = 0.0;
	float3 normalLightDir = normalize(_dirLightDir);
	float3 invLightDir = - normalLightDir;
	float memWorldNormal = normalize(worldNormal);
	lightFactor = saturate(dot(invLightDir, memWorldNormal));
	return lightFactor;
}