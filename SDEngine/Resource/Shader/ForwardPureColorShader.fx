SamplerState SampleType:register(s0);   //采样方式


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
	float3 W_Normal:NORMAL; 
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.W_Normal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //此事世界逆转置矩阵的第四行本来就没啥用
	outa.W_Normal = normalize(outa.W_Normal);
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	
	float4 color;
	float3 lightDir = normalize(-lightDir);
	float3 worldNormal = normalize(outa.W_Normal);
	//float diffuseFactor = saturate(dot(worldNormal, lightDir));
	color.rgb = lightColor.xyz * surfaceColor.xyz;
	color.a = 0.6;
	return color;
}