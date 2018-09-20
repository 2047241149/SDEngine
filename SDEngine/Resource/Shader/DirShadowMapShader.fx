Texture2D WorldPosTex:register(t0);
Texture2D LightDepthMap:register(t1);

SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);
SamplerState clampPointSample:register(s2);

static const int PCF_KERNEL_COUNT = 9;

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
	float3 cameraPos;
	float pad1;
};

cbuffer CBShadow:register(b1)
{
	matrix dirView;
	matrix dirProj;
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

float2 texSize(Texture2D tex)
{
	uint texWidth, texHeight;
	tex.GetDimensions(texWidth, texHeight);
	return float2(texWidth, texHeight);
}

VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = float4(ina.Pos.xy,1.0,1.0);
	outa.Tex= ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	float3 worldPos = WorldPosTex.Sample(clampLinearSample, outa.Tex).xyz;
	float4 lightSpaceWSPos = mul(float4(worldPos, 1.0f), dirView);
	lightSpaceWSPos = mul(lightSpaceWSPos, dirProj);
	float4 color;
	float2 lightDepthUV = (lightSpaceWSPos.xy / lightSpaceWSPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
	float2 pcf_kernel[PCF_KERNEL_COUNT] =
	{
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(-1, 1), float2(0, 1), float2(1, 1),
	};
	/*bool isOutOfOrthoFrustum = (lightDepthUV.x >= 0.0) && (lightDepthUV.x <= 1.0) && (lightDepthUV.y >= 0.0) && (lightDepthUV.y <= 1.0);
	float nearestDepth = LightDepthMap.Sample(clampPointSample, lightDepthUV).r;
	float z = lightSpaceWSPos.z / lightSpaceWSPos.w;
	bool isShadowed = z > (nearestDepth + 0.003);
	float4 color = (int)isShadowed * (int)isOutOfOrthoFrustum ? float4(0.0, 0.0, 0.0, 1.0) : float4(1.0, 1.0, 1.0, 1.0);*/
	float2 lightDepthMapTexSize = 1.0 / texSize(LightDepthMap);
	if ((lightDepthUV.x >= 0.0) && (lightDepthUV.x <= 1.0) && (lightDepthUV.y >= 0.0) && (lightDepthUV.y <= 1.0))
	{
		for (int index = 0; index < PCF_KERNEL_COUNT; ++index)
		{
			float2 uv = lightDepthUV.xy + pcf_kernel[index] * lightDepthMapTexSize;
			float nearestDepth = LightDepthMap.SampleLevel(clampPointSample, uv, 0).r;
			float z = lightSpaceWSPos.z / lightSpaceWSPos.w;
			bool isShadowed = z > (nearestDepth + 0.003);
			color += isShadowed ? float4(0.0, 0.0, 0.0, 1.0) : float4(1.0, 1.0, 1.0, 1.0);
		}

		color /= (float)PCF_KERNEL_COUNT;
	
	}
	else
	{
		color = float4(1.0, 1.0, 1.0, 1.0);
	}

	return color;
}