
#define THREAD_GROUD_X 16
#define THREAD_GROUD_Y 12
#define THREAD_GROUD_Z 2

#define GROUD_THREAD_TOTAL_NUM THREAD_GROUD_X * THREAD_GROUD_Y * THREAD_GROUD_Z

struct Cluter
{
	float4 minPoint;
	float4 maxPoint;
};

struct LightGrid
{
	float offset;
	float count;
};

struct PointLight
{
	float3 pos;
	float3 color;
	float radius;
	float4 attenuation;
};

cbuffer CBCommon:register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float lightCount;
};

cbuffer CBMatrix:register(b1)
{
	matrix View;
	matrix ProjInv;
};

StructuredBuffer<PointLight> PointLights : register(t0);
RWStructuredBuffer<Cluter> ClusterList : register(u0);
RWStructuredBuffer<LightGrid> LightGridList : register(u1);
RWStructuredBuffer<float> GlobalLightIndexList : register(u2);
RWStructuredBuffer<uint> globalIndexCount: register(u3);
RWStructuredBuffer<float> ClusterActiveList: register(u4);
//groupshared PointLight sharedLights[GROUD_THREAD_TOTAL_NUM];

float GetSqdisPointAABB(float3 pos, uint cluterIndex)
{
	float sqDistance = 0.0;

	Cluter cluter = ClusterList[cluterIndex];

	for (int i = 0; i < 3; ++i)
	{
		float v = pos[i];

		if (v < cluter.minPoint[i])
		{
			float diff = cluter.minPoint[i] - v;
			sqDistance += diff * diff;
		}

		if (v > cluter.maxPoint[i])
		{
			float diff =  v - cluter.maxPoint[i];
			sqDistance += diff * diff;
		}
	}

	return sqDistance;
}

bool TestSphereAABB(uint lightIndex, uint cluterIndex)
{
	PointLight light = PointLights[lightIndex];
	//PointLight light = sharedLights[lightIndex];
	float radius = light.radius;
	float3 spherePos = mul(float4(light.pos, 1.0), View).xyz;
	float sqDistance = GetSqdisPointAABB(spherePos, cluterIndex);
	return sqDistance <= (radius * radius);
}

[numthreads(THREAD_GROUD_X, THREAD_GROUD_Y, THREAD_GROUD_Z)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint threadCount = GROUD_THREAD_TOTAL_NUM;
	uint lightCountInt = (uint)lightCount;
	uint passCount = (lightCountInt + threadCount - 1) / threadCount;
	uint clusterIndex = groupIndex + threadCount * groupId.z;
	uint visibleLightCount = 0;

	//Unvalid Cluster
	if (ClusterActiveList[clusterIndex] == 0.0)
	{
		LightGridList[clusterIndex].offset = (float)globalIndexCount[0];
		LightGridList[clusterIndex].count = 0.0;
		return;
	}

	//one cluster max light num <= GROUD_THREAD_TOTAL_NUM
	uint visibleLightIndexs[GROUD_THREAD_TOTAL_NUM];

	for (uint passIndex = 0; passIndex < passCount; ++passIndex)
	{
		uint lightIndex = passIndex * threadCount + groupIndex;
		//sharedLights[groupIndex] = PointLights[lightIndex];
		//GroupMemoryBarrierWithGroupSync();

		for (uint light = 0; light < threadCount; ++light)
		{
			uint lightRealIndex = light + passIndex * threadCount;

			if (lightRealIndex < lightCountInt && TestSphereAABB(light, clusterIndex))
			{
				visibleLightIndexs[visibleLightCount] = lightRealIndex;
				visibleLightCount += 1;
			}
		}
	}

	//We want all thread groups to have completed the light tests before continuing
	GroupMemoryBarrierWithGroupSync();
	
	uint offset;
	InterlockedAdd(globalIndexCount[0], visibleLightCount, offset);

	for (uint i = 0; i < visibleLightCount; ++i)
	{
		GlobalLightIndexList[offset + i] = visibleLightIndexs[i];
	}

	LightGridList[clusterIndex].offset = (float)offset;
	LightGridList[clusterIndex].count = (float)visibleLightCount;
}