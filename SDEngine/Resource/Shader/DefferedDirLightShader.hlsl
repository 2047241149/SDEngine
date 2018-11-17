Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SpecularTex:register(t2);
Texture2D DirLightShadowMap:register(t3);
Texture2D SSAORT:register(t4);

SamplerState clampLinearSample:register(s0);  

cbuffer CBMatrix:register(b0)
{
	float3 cameraPos;
};

cbuffer CBDirLight:register(b1)
{
	float4 lightColor;  //w存在的为强度
	float3 lightDir;
	float3 ambientLight;
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




float4 PS(VertexOut outa) : SV_Target
{
	float4 color = float4(0.0, 0.0, 0.0, 0.0);

	//diffuseLight
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, outa.Tex).xyz;
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, outa.Tex).xyz;
	worldNormal = normalize(worldNormal);
	float specular = SpecularTex.Sample(clampLinearSample, outa.Tex).x;
	float3 invLightDir = -normalize(lightDir);
	float diffuseFactor = saturate(dot(worldNormal, invLightDir));

	//计算specular
	float3 viewDir = normalize(cameraPos - worldPos);
	float3 halfDir = normalize(invLightDir + viewDir);
	float specularFactor = pow(saturate(dot(halfDir, worldNormal)), 32) * specular * 0.05;

	float ao = SSAORT.Sample(clampLinearSample, outa.Tex).r;

	//利用DirLightShadowMap
	float3 shadowFactor = DirLightShadowMap.Sample(clampLinearSample, outa.Tex).rgb;
	color = float4(ambientLight * ao + lightColor.xyz * diffuseFactor * shadowFactor * lightColor.w, 1.0);
	color.w = specularFactor;

	return color;
}


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = float4(ina.Pos.xy, 1.0, 1.0);
	outa.Tex = ina.Tex;
	return outa;
}