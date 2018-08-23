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
	float radius;
	float3 attenuation;
	float pad1;
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
	float4 color = float4(0.0, 0.0, 0.0, 0.0);
	float3 diffuse = DiffuseTex.Sample(wrapLinearSample, uv).xyz;
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, uv).xyz;
	float3 worldNormal = WorldNormalTex.Sample(clampLinearSample, uv).xyz;
	worldNormal = normalize(worldNormal);
	float specular = SpecularTex.Sample(wrapLinearSample, uv).x;

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
	float attenua = 1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
	color = color * attenua;
	float correctGamma = 1.0 / 2.2;
	color = pow(color, float4(correctGamma, correctGamma, correctGamma, 0.0));
	color.a = 1.0f;

	return color;
}