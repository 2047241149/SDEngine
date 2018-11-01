SamplerState SampleType:register(s0);   //采样方式


cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
};

cbuffer CBEveryFrame:register(b1)
{
	float4 surfaceColor;
}

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
	float3 worldPos:POSITION;
};

struct PixelOut
{
	float4 diffuse:SV_Target1;
	float4 worldPos:SV_Target2;
	float4 worldNormal:SV_Target3;
	float4 specular:SV_Target4;
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), World);
	outa.worldPos = outa.Pos.xyz;
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.W_Normal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //此事世界逆转置矩阵的第四行本来就没啥用
	outa.W_Normal = normalize(outa.W_Normal);
	return outa;
}


PixelOut PS(VertexOut outa) : SV_Target
{
	
	PixelOut pixelOut;

	//diffuse
	pixelOut.diffuse = surfaceColor;

	//worldPos
	pixelOut.worldPos = float4(outa.worldPos, 1.0);

	//worldNormal
	pixelOut.worldNormal = float4(normalize(outa.W_Normal), 1.0);

	//specular
	pixelOut.specular = float4(1.0, 1.0, 1.0, 1.0);

	return pixelOut;
}