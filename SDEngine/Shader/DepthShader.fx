Texture2D ShaderTexture:register(t0);  //纹理资源
SamplerState SampleType:register(s0);   //采样方式

//VertexShader
cbuffer CBMatrix:register(b0)
{
	matrix UIView;
	matrix UIOrtho;
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
	float4 color;

	color = ShaderTexture.Sample(SampleType, outa.Tex);
	return float4(color.r, color.r, color.r, 1.0f);

}