Texture2D DiffuseTex:register(t0);  
Texture2D NormalTex:register(t1);
SamplerState SampleWrapLinear:register(s0);
SamplerState SampleClampPoint:register(s1);

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
	float3 cameraPos;
	float pad1;
};

cbuffer CBEveryFrame:register(b1)
{
	float4 surfaceColor;
}

cbuffer CBDirLight : register(b2)
{
	float4 lightColor;
	float3 lightDir;
	float3 ambientLight;
	float2 pad;
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
	float3 worldPos:POSITION0;
	float3 WorldNormal:NORMAL;
	float3 WorldTangent:TANGENT;
	float2 Tex:TEXCOORD0;
};



VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);

	outa.worldPos = outa.Pos.xyz;
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.WorldNormal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //此事世界逆转置矩阵的第四行本来就没啥用
	outa.WorldNormal = normalize(outa.WorldNormal);

	outa.WorldTangent = mul(ina.Tangent, (float3x3)World);
	outa.WorldTangent = normalize(outa.WorldTangent);
	
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float4 color = float4(0.0, 0.0, 0.0, 1.0);

	float3 normal = NormalTex.Sample(SampleWrapLinear, outa.Tex).xyz;
	normal = normal * 2.0 - 1.0;

	float3 N = normalize(outa.WorldNormal);
	float3 T = normalize(outa.WorldTangent);
	float3 B = normalize(cross(N, T));

	float3 worldNormal;
	worldNormal.x = normal.x * T.x + normal.y * B.x + normal.z * N.x;
	worldNormal.y = normal.x * T.y + normal.y * B.y + normal.z * N.y;
	worldNormal.z = normal.x * T.z + normal.y * B.z + normal.z * N.z;
	worldNormal = normalize(worldNormal);

	//float3 worldNormal = normalize(outa.WorldNormal);

	//计算diffuseLight
	float3 invLightDir = -normalize(lightDir);
	float diffuseFactor = saturate(dot(worldNormal, invLightDir));
	float3 diffuse = DiffuseTex.Sample(SampleWrapLinear, outa.Tex).xyz;
	diffuse = pow(diffuse, float4(2.0f, 2.0f, 2.0f, 0.0f));


	//SpecularLight
	float3 viewDir = normalize(cameraPos - outa.worldPos);
	float3 halfDir = normalize(invLightDir + viewDir);
	float spec = pow(saturate(dot(halfDir, worldNormal)), 32);
	float3 specular = float3(spec, spec, spec);

	color = float4((ambientLight + lightColor.xyz * diffuseFactor) * diffuse  + specular * 0.0f, 1.0);

	float correctGamma = 1.0 / 2.0;

	color = pow(color, float4(correctGamma, correctGamma, correctGamma, 0.0));

	return color;
}