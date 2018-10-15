Texture2D WorldPosTex:register(t0);
Texture2D CascadeLightDepthMap:register(t5);


SamplerState wrapLinearSample:register(s0);
SamplerState clampLinearSample:register(s1);
SamplerState clampPointSample:register(s2);

static const int CASCADE_SHADOW_NUM = 3;
static const int PCF_KERNEL_COUNT = CASCADE_SHADOW_NUM * CASCADE_SHADOW_NUM;


static const float4 ARRAY_DEBUG_COLOR[CASCADE_SHADOW_NUM] = 
{
	float4(1.0, 0.0, 0.0, 1.0),
	float4(0.0, 1.0, 0.0, 1.0),
	float4(0.0, 0.0, 1.0, 1.0),
};

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
	float4 lightSpacePos = mul(float4(worldPos, 1.0f), dirView);
	float4 color;
	int nCascadeIndex = 0;
	int nCascadeFound = 0;

	float2 pcf_kernel[PCF_KERNEL_COUNT] =
	{
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(-1, 1), float2(0, 1), float2(1, 1),
	};

	//U方向和V方向步进的单位不同
	float2 lightDepthMapTexSize = 1.0 / texSize(CascadeLightDepthMap);
	float2 lightDepthUV = float2(0.0, 0.0);
	float4 lightSpaceWSPos;

	for (int index = 0; index < CASCADE_SHADOW_NUM &&  0 == nCascadeFound; ++index)
	{
		lightSpaceWSPos = mul(lightSpacePos, arrayDirProj[index]);
		lightDepthUV = (lightSpaceWSPos.xy / lightSpaceWSPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
		
		if (min(lightDepthUV.x, lightDepthUV.y) > 0.0 && max(lightDepthUV.x, lightDepthUV.y) < 1.0)
		{
			nCascadeIndex = index;
			nCascadeFound = 1;
		}
	}

	float4 debugColor = ARRAY_DEBUG_COLOR[nCascadeIndex];

	for (int nKernelIndex = 0; nKernelIndex < PCF_KERNEL_COUNT; ++nKernelIndex)
	{
		float2 uv = float2(((lightDepthUV.x + (float)nCascadeIndex) / (float)CASCADE_SHADOW_NUM), lightDepthUV.y)
		+ pcf_kernel[nKernelIndex] * lightDepthMapTexSize;
		float nearestDepth = CascadeLightDepthMap.SampleLevel(clampPointSample, uv, 0).r;
		float z = lightSpaceWSPos.z / lightSpaceWSPos.w;
		bool isShadowed = z > (nearestDepth + 0.003);
		color += isShadowed ? float4(0.0, 0.0, 0.0, 1.0) : float4(1.0, 1.0, 1.0, 1.0);
	}
	
	color /= (float)PCF_KERNEL_COUNT;	

	color = color * debugColor;

	return color;
}