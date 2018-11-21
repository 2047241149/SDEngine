Texture2D ShaderTexture:register(t0);  //纹理资源
SamplerState SampleType:register(s0);   //采样方式


//VertexShader
cbuffer CBMatrix:register(b0)
{
	matrix UIView;
	matrix UIOrtho;
	float farPlane;
	float nearPlane;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float2 Tex:TEXCOORD;  //多重纹理可以用其它数字

};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};


VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), UIView);
	outa.Pos = mul(outa.Pos, UIOrtho);

	outa.Tex= ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{

	float depth = ShaderTexture.Sample(SampleType, outa.Tex).r;
	depth = DepthBufferConvertToLinear(depth);
	depth = depth / farPlane;
	return float4(depth, depth, depth, 1.0f);
}