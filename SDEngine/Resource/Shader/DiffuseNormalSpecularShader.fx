Texture2D DiffuseTexture:register(t0);  //纹理资源
Texture2D NormalTexture:register(t1);  //纹理资源
Texture2D SpecularTexture:register(t2);  //纹理资源
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
	float2 Tex:TEXCOORD0;
	float3 worldPos:TEXCOORD1;
	float3 W_Normal:NORMAL;  //世界空间的法线
	float3 W_Tangent:TANGENT;
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
	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.worldPos = outa.Pos.xyz;
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.W_Normal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //此事世界逆转置矩阵的第四行本来就没啥用
	outa.W_Normal = normalize(outa.W_Normal);
	outa.Tex= ina.Tex;
	return outa;
}


PixelOut PS(VertexOut outa) : SV_Target
{

	PixelOut pixelOut;
	float4 diffuse; //采集的纹理颜色
	float3 normal;

	//diffuse
	pixelOut.diffuse = DiffuseTexture.Sample(SampleWrapLinear, outa.Tex);

	//worldPos
	pixelOut.worldPos = float4(outa.worldPos, 1.0);

	//worldNormal
	normal = NormalTexture.Sample(SampleWrapLinear, outa.Tex).xyz;
	normal.xyz = normal.xyz * 2.0 - 1.0;
	float3 N = normalize(outa.W_Normal);
	float3 T = normalize(outa.W_Tangent);
	float3 B = normalize(cross(N, T));

	//基底映射TBN
	float3 worldNormal;
	worldNormal.x = normal.x * T.x + normal.y * B.x + normal.z * N.x;
	worldNormal.y = normal.x * T.y + normal.y * B.y + normal.z * N.y;
	worldNormal.z = normal.x * T.z + normal.y * B.z + normal.z * N.z;
	worldNormal = normalize(worldNormal);

	pixelOut.worldNormal = float4(worldNormal, 1.0);
	//specular
	pixelOut.specular = SpecularTexture.Sample(SampleWrapLinear, outa.Tex);

	return pixelOut;
}