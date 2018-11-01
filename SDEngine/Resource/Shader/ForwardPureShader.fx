


cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
};


cbuffer CBDirLight : register(b1)
{
	float4 lightColor;
	float3 lightDir;
	float3 ambientLight;
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
	color = float4(1.0, 0.0, 0.0, 0.5);

	return color;
}