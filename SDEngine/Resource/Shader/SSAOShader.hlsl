Texture2D WorldPosTex:register(t0);
Texture2D WorldNormalTex:register(t1);
Texture2D SSaoNoiseTexture:register(t2);
SamplerState clampPointSample:register(s0);
SamplerState wrapPointSample:register(s1);

static const int SSAO_NUM = 64;
static const float bias = 0.0;
static const float radius = 1.5f;
	
cbuffer CBSSAO:register(b0)
{
	matrix View;
	matrix Proj;
	matrix ViewInvTranspose;
	float3 ssaoSamples[SSAO_NUM];
};

float2 texSize(Texture2D tex)
{
	float texWidth, texHeight;
	tex.GetDimensions(texWidth, texHeight);
	return float2(texWidth, texHeight);
}




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
	outa.Pos = float4(ina.Pos.xy, 1.0, 1.0);
	outa.Tex = ina.Tex;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float3 worldPos = WorldPosTex.Sample(clampPointSample, outa.Tex).rgb;
	float3 worldNormal = WorldNormalTex.Sample(clampPointSample, outa.Tex).rgb;
	float3 viewPos = mul(float4(worldPos, 1.0), View).xyz;
	float3 viewNormal = mul(worldNormal, (float3x3)ViewInvTranspose).xyz;
	float2 noiseTextureCoordScale = texSize(WorldPosTex) / 4.0;
	float3 randomVec = SSaoNoiseTexture.Sample(wrapPointSample, outa.Tex * noiseTextureCoordScale).rgb;
	float3 tangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
	float3 bitangent = normalize(cross(viewNormal, tangent));
	float3x3 TBN = float3x3(tangent, bitangent, viewNormal);
	float occulusion = 0.0;

	for (int i = 0; i < SSAO_NUM; ++i)
	{
		float3 sampleVec = mul(ssaoSamples[i], TBN);
		sampleVec = sampleVec * radius + viewPos;

		float4 offset = float4(sampleVec, 1.0);
		offset = mul(offset, Proj);
		offset.xyz /= offset.w;
		offset.xy = offset.xy * float2(0.5, -0.5) + float2(0.5, 0.5);

		float3 samplPos = WorldPosTex.Sample(clampPointSample, offset.xy).xyz;
		float sampleDepth = mul(float4(samplPos, 1.0), View).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / (abs(sampleDepth - viewPos.z)));

		//在角落处往往形成sampleDepth < sampleVec.z + bias
		//opengl和dx11的相机空间的Z值相反的
		occulusion += ((sampleDepth + bias < sampleVec.z) ? 1.0 : 0.0) * rangeCheck;
	}

	occulusion = 1.0 - (occulusion / SSAO_NUM);

	return float4(occulusion, occulusion, occulusion, 1.0);
}


