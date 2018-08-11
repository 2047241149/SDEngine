Texture2D DiffuseTex:register(t0); 
Texture2D WorldPosTex:register(t1);
Texture2D WorldNormalTex:register(t2);
Texture2D SpecularTex:register(t3);

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
	float3 attenuation;
	float radius;
	float pad1;
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
	outa.Pos = float4(ina.Pos.xy,1.0,1.0);
	outa.Tex= ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float4 color = float4(0.0, 0.0, 0.0, 0.0);

	float3 diffuse = DiffuseTex.Sample(wrapLinearSample, outa.Tex).xyz;
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, outa.Tex).xyz;
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, outa.Tex).xyz;
	worldNormal = normalize(worldNormal);
	float specular = SpecularTex.Sample(wrapLinearSample, outa.Tex).x;

	//º∆À„Diffuse
	float3 pixelToLightDir = lightPos - worldPos;
	float distance = length(pixelToLightDir);
	pixelToLightDir = normalize(pixelToLightDir);
	float diffuseFactor = saturate(dot(worldNormal, pixelToLightDir));

	//º∆À„specular
	float3 viewDir = normalize(cameraPos - worldPos);
	float3 halfDir = normalize(pixelToLightDir + viewDir);
	float specularFactor = pow(saturate(dot(halfDir, worldNormal)), 32) * specular;

	color = float4((lightColor.xyz * diffuseFactor) * diffuse, 1.0);
	color = color + float4(specularFactor, specularFactor, specularFactor, 1.0);
	color = color / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
	float correctGamma = 1.0 / 1.5;
	color = pow(color, float4(correctGamma, correctGamma, correctGamma, 0.0));
	color.a = 1.0f;


	return color;
}