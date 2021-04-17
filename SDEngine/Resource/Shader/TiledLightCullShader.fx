

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
	matrix ProjInv;
};

cbuffer CBCommon:register(b2)
{
	float ScreenWidth;
	float ScreenHeight;
};

uint GetNumTilesX()
{
	return (uint)((ScreenWidth + GroundThreadSize - 1) / (float)GroundThreadSize);
}

uint GetNumTilesY()
{
	return (uint)((ScreenHeight + GroundThreadSize - 1) / (float)GroundThreadSize);
}

//b和c为ViewSpace空间的点,顺时针方向
//normalize(cross( b-a, c-a )),  a = float3(0,0,0)
float3 CreatePlaneEquation(float3 b, float3 c)
{
	return normalize(cross(b, c));
}

//求某个点和平面的有向距离
// dot(eqn.xyz,p) + eqn.w, 相机空间: eqn.w = 0
float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
	return dot(eqn, p);
}

bool TestFrustumSides(float3 lightPos, float radius, float3 plane0, float3 plane1, float3 plane2, float3 plane3)
{
	float3 aa = lightPos;
	float a1 = GetSignedDistanceFromPlane(lightPos, plane0);
	float a2 = GetSignedDistanceFromPlane(lightPos, plane1);
	float a3 = GetSignedDistanceFromPlane(lightPos, plane2);
	float a4 = GetSignedDistanceFromPlane(lightPos, plane3);
	bool intersectingOrInside0 = a1 < radius;
	bool intersectingOrInside1 = a2 < radius;
	bool intersectingOrInside2 = a3 < radius;
	bool intersectingOrInside3 = a4 < radius;

	return (intersectingOrInside0 && intersectingOrInside1 && intersectingOrInside2
		&& intersectingOrInside3);
}

float4 ConvertProjToView(float4 p)
{
	p = mul(p, ProjInv);
	p /= p.w;
	return p;
}

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};


Texture2D<float4> DepthTex:register(t0);
StructuredBuffer<PointLight> PointLights : register(t1);
RWTexture2D<float4> OutputTexture : register(u0);
groupshared uint minDepthInt;
groupshared uint maxDepthInt;
groupshared uint visibleLightCount = 0;
groupshared uint visibleLightIndices[1024];

[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{

	//(1)计算每个Tiled的Frustum
	//构建NDC空间的点,转换到View空间,并算出frustum的四个面向量
	float3 frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3;
	uint tileResWidth = GroundThreadSize * GetNumTilesX();
	uint tileResHeight = GroundThreadSize * GetNumTilesY();
	uint pxm = GroundThreadSize * groupId.x;
	uint pym = GroundThreadSize * groupId.y;
	uint pxp = GroundThreadSize * (groupId.x + 1);
	uint pyp = GroundThreadSize * (groupId.y + 1);

	// four corners of the tile, clockwise from top-left
	float3 frustum0 = ConvertProjToView(float4(pxm / (float)tileResWidth*2.f - 1.f, (tileResHeight - pym) / (float)tileResHeight*2.f - 1.f, 1.f, 1.f)).xyz;
	float3 frustum1 = ConvertProjToView(float4(pxp / (float)tileResWidth*2.f - 1.f, (tileResHeight - pym) / (float)tileResHeight*2.f - 1.f, 1.f, 1.f)).xyz;
	float3 frustum2 = ConvertProjToView(float4(pxp / (float)tileResWidth*2.f - 1.f, (tileResHeight - pyp) / (float)tileResHeight*2.f - 1.f, 1.f, 1.f)).xyz;
	float3 frustum3 = ConvertProjToView(float4(pxm / (float)tileResWidth*2.f - 1.f, (tileResHeight - pyp) / (float)tileResHeight*2.f - 1.f, 1.f, 1.f)).xyz;
	frustumEqn0 = CreatePlaneEquation(frustum0, frustum1);
	frustumEqn1 = CreatePlaneEquation(frustum1, frustum2);
	frustumEqn2 = CreatePlaneEquation(frustum2, frustum3);
	frustumEqn3 = CreatePlaneEquation(frustum3, frustum0);

	GroupMemoryBarrierWithGroupSync();

	//(2)计算每个Tiled的世界位置的MaxZ和MinZ
	float depth = DepthTex[dispatchThreadId.xy].r;
	float viewZ = DepthBufferConvertToLinear(depth);
	uint depthInt = asuint(viewZ);
	minDepthInt = 0xFFFFFFFF;
	maxDepthInt = 0;
	GroupMemoryBarrierWithGroupSync();

	if (depth != 0.0)
	{
		InterlockedMin(minDepthInt, depthInt);
		InterlockedMax(maxDepthInt, depthInt);
	}

	GroupMemoryBarrierWithGroupSync();

	float minViewZ = asfloat(minDepthInt);
	float maxViewZ = asfloat(maxDepthInt);

	//(3)计算和每个Tiled相交的点光源数量,并记录它们的索引
	uint threadCount = GroundThreadSize * GroundThreadSize;
	uint passCount = (int(lightCount) + threadCount - 1) / threadCount;

	for (uint i = 0; i < passCount; ++i)
	{
		uint lightIndex = i * threadCount + groupIndex;
		lightIndex = min(lightIndex, lightCount);
		PointLight light = PointLights[lightIndex];
		float3 viewLightPos = mul(float4(light.pos, 1.0), View).xyz;
		if(TestFrustumSides(viewLightPos, light.radius, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3))
		{
			if (minViewZ - viewLightPos.z < light.radius && viewLightPos.z - maxViewZ < light.radius)
			{
				uint offset;
				InterlockedAdd(visibleLightCount, 1, offset);
				visibleLightIndices[offset] = lightIndex;
			}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	//(4)计算和每个Tiled的光源贡献值
	float maxLightCount = 40.0;
	float value = float(visibleLightCount) / maxLightCount;
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	if (value == 0)
	{
		color = float4(0.0, 0.0, 0.0, 1.0);
	}
	else if (value <= 0.1)
	{
		color = float4(1.0, 0.0, 0.0, 1.0);
	}
	else if (value <= 0.2)
	{
		color = float4(0.0, 1.0, 0.0, 1.0);
	}
	else if(value <= 0.4)
	{
		color = float4(0.0, 0.0, 1.0, 1.0);
	}
	else if (value <= 0.6)
	{
		color = float4(1.0, 1.0, 0.0, 1.0);
	}
	else
	{
		color = float4(1.0, 1.0, 1.0, 1.0);
	}

	OutputTexture[dispatchThreadId.xy] = color;
}