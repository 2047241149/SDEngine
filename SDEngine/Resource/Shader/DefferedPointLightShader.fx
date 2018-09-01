Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SpecularTex:register(t2);

SamplerState wrapLinearSample:register(s0);  
SamplerState clampLinearSample:register(s1);  

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
	float3 cameraPos;
	float pad;
};


cbuffer CBPointLight:register(b1)
{
	float4 lightColor;
	float3 lightPos;
	float radius;
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
	float4 light = float4(0.0, 0.0, 0.0, 0.0);
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, uv).xyz;
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, uv).xyz;
	worldNormal = normalize(worldNormal);
	float specular = SpecularTex.Sample(wrapLinearSample, uv).x;

	//º∆À„DiffuseLight
	float3 pixelToLightDir = lightPos - worldPos;
	float distance = length(pixelToLightDir);
	float distanceSquad = max(0, distance - radius / 2.0);
	pixelToLightDir = normalize(pixelToLightDir);
	float diffuseFactor = saturate(dot(worldNormal, pixelToLightDir));

	//º∆À„specularLight
	float3 viewDir = normalize(cameraPos - worldPos);
	float3 halfDir = normalize(pixelToLightDir + viewDir);
	float specularFactor = pow(saturate(dot(halfDir, worldNormal)), 32) * specular;

	light = float4(lightColor.xyz *  diffuseFactor * lightColor.w, specularFactor);
	float attenua = 1.0 / (attenuation.x + attenuation.y * distance + distance * distance * attenuation.w + attenuation.z * distanceSquad * distanceSquad);
	light = light * attenua;

	return light;
}