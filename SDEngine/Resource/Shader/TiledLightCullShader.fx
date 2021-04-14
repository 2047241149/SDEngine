

#define GroundThreadSize 16


//点光源
struct PointLight
{
	float3 pos;
	float3 color;
	float radius;
};

cbuffer CBLight:register(b0)
{
	float lightCount;
	float farPlane;
	float nearPlane;
};

cbuffer CBMatrix:register(b1)
{
	matrix View;
};


float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};

bool isInsect(PointLight light, float minDepth, float maxDepth)
{
	float viewMinZ = DepthBufferConvertToLinear(minDepth);
	float viewMaxZ = DepthBufferConvertToLinear(maxDepth);
	float viewLightPosZ = mul(float4(light.pos, 1.0), View).z;
	float minLightZ = viewLightPosZ - light.radius;
	float maxLightZ = viewLightPosZ + light.radius;
	if (maxLightZ <= viewMinZ || minLightZ >= viewMaxZ)
		return false;

	return true;
}

Texture2D<float4> DepthTex:register(t0);
StructuredBuffer<PointLight> PointLights : register(t1);
RWTexture2D<float4> OutputTexture : register(u0);
groupshared int minDepthInt;
groupshared int maxDepthInt;
groupshared uint visibleLightCount = 0;
groupshared uint visibleLightIndices[1024];

[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	//(1)计算每个Tiled的世界位置的MaxZ和MinZ
	float depth = DepthTex[dispatchThreadId.xy].r;
	uint depthInt = asuint(depth);
	minDepthInt = 0xFFFFFFFF;
	maxDepthInt = 0;
	GroupMemoryBarrierWithGroupSync();

	InterlockedMin(minDepthInt, depthInt);
	InterlockedMax(maxDepthInt, depthInt);
	GroupMemoryBarrierWithGroupSync();

	float minDepth = asfloat(minDepthInt);
	float maxDepth = asfloat(maxDepthInt);

	//(2)计算和每个Tiled相交的点光源数量,并记录它们的索引
	uint threadCount = GroundThreadSize * GroundThreadSize;
	uint passCount = (int(lightCount) + threadCount - 1) / threadCount;

	for (uint i = 0; i < passCount; ++i)
	{
		uint lightIndex = i * threadCount + groupIndex;
		lightIndex = min(lightIndex, lightCount);

		if(isInsect(PointLights[lightIndex], minDepth, maxDepth))
		{
			uint offset;
			InterlockedAdd(visibleLightCount, 1, offset);
			visibleLightIndices[offset] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	//(3)计算和每个Tiled的光源贡献值
	float maxLightCount = 200.0;
	float value = float(visibleLightCount) / maxLightCount;
	OutputTexture[dispatchThreadId.xy] = float4(value, value, value, 1.0);
}