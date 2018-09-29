Texture2D WorldPosTex:register(t0);
Texture2D CascadeLightDepthMap:register(t1);



SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);
SamplerState clampPointSample:register(s2);

static const int PCF_KERNEL_COUNT = 9;
static const int CASCADE_SHADOW_NUM = 3;

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
	matrix arrayDirProj[CASCADE_SHADOW_NUM];
	float3 cameraZ;
	float shadowBias;
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
	float texWidth, texHeight;
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
	float4 viewSpacePos = mul(float4(worldPos, 1.0f), View);
	float4 lightSpaceWSPos = mul(float4(worldPos, 1.0f), dirView);
	float4 color;
	int projMatrixIndex = 0;
	float4 debugColor = float4(1.0, 1.0, 1.0, 1.0);
	if (viewSpacePos.z < cameraZ.x)
	{
		projMatrixIndex = 0;
		debugColor = float4(1.0, 0.0, 0.0, 1.0);
	}
	else if (viewSpacePos.z < cameraZ.y)
	{
		projMatrixIndex = 1;
		debugColor = float4(0.0, 1.0, 0.0, 1.0);
	}
	else if (viewSpacePos.z < cameraZ.z)
	{
		projMatrixIndex = 2;
		debugColor = float4(0.0, 0.0, 1.0, 1.0);
	}
	else
	{
		return float4(1.0, 1.0, 1.0, 1.0);
	}
	
	lightSpaceWSPos = mul(lightSpaceWSPos, arrayDirProj[projMatrixIndex]);
	float2 lightDepthUV = (lightSpaceWSPos.xy / lightSpaceWSPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
	float2 pcf_kernel[PCF_KERNEL_COUNT] =
	{
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(-1, 1), float2(0, 1), float2(1, 1),
	};

	//U方向和V方向步进的单位不同
	float2 lightDepthMapTexSize = 1.0 / texSize(CascadeLightDepthMap);


	if ((lightDepthUV.x >= 0.0) && (lightDepthUV.x <= 1.0) && (lightDepthUV.y >= 0.0) && (lightDepthUV.y <= 1.0))
	{
		for (int index = 0; index < PCF_KERNEL_COUNT; ++index)
		{
			float2 uv = float2(((lightDepthUV.x + (float)projMatrixIndex) / (float)CASCADE_SHADOW_NUM), lightDepthUV.y)
			+ pcf_kernel[index] * lightDepthMapTexSize;
			float nearestDepth = CascadeLightDepthMap.SampleLevel(clampPointSample, uv, 0).r;
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

	color = color * debugColor;

	return color;
}