struct Cluter
{
	float4 minPoint;
	float4 maxPoint;
};

cbuffer CBCommon:register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float farPlane;
	float nearPlane;
	float4 tileSizes;
};

cbuffer CBMatrix:register(b1)
{
	matrix View;
	matrix ProjInv;
};

float4 ConvertNDCToView(float4 p)
{
	p = mul(p, ProjInv);
	p /= p.w;
	return p;
}

//Screen-NDC-View
float4 ScreenToView(float4 screenPos)
{
	float2 tex = screenPos.xy / float2(ScreenWidth, ScreenHeight);
	// XYndc = (tex - float2(0.5, 0.5)) * (2, -2)
	float4 ndc = float4(tex.x * 2.0 - 1.0, 1.0 - 2.0 * tex.y, screenPos.z, screenPos.w);
	return ConvertNDCToView(ndc);
}

//相似三角形求解
float3 LineIntersectionToZPlane(float3 a, float3 b, float z)
{
	float3 normal = float3(0.0, 0.0, 1.0);
	float3 ab = b - a;
	float t = (z - dot(normal, a)) / dot(normal, ab);
	float3 result = a + t * ab;
	return result;
}

RWStructuredBuffer<Cluter> ClusterList : register(u0);

[numthreads(1, 1, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const float3 eyePos = float3(0.0, 0.0, 0.0);
	uint tileSizePx = (uint)tileSizes.w;
	uint tileIndex = groupId.x + groupId.y * (uint)tileSizes.x + groupId.z * (uint)tileSizes.x * (uint)tileSizes.y;

	//Calculate the min and max point in screen, far plane, near plane exit error(forever zero)
	float4 maxPointSs = float4(float2(groupId.x + 1, groupId.y + 1) * tileSizePx, 1.0, 1.0);
	float4 minPointSs = float4(groupId.xy * tileSizePx, 1.0, 1.0);

	//MinPoint and MaxPoint of the cluster in view space(nearest plane, ndc pos.w = 0.0)
	float3 maxPointVs = ScreenToView(maxPointSs).xyz;
	float3 minPointVs = ScreenToView(minPointSs).xyz;

	//Near and far values of the cluster in view space, the split cluster method from siggraph 2016 idtech6
	float tileNear = nearPlane * pow(farPlane / nearPlane, groupId.z / tileSizes.z);
	float tileFar = nearPlane * pow(farPlane / nearPlane, (groupId.z + 1) / tileSizes.z);

	//find cluster min/max 4 point in view space
	float3 minPointNear = LineIntersectionToZPlane(eyePos, minPointVs, tileNear);
	float3 minPointFar = LineIntersectionToZPlane(eyePos, minPointVs, tileFar);
	float3 maxPointNear = LineIntersectionToZPlane(eyePos, maxPointVs, tileNear);
	float3 maxPointFar = LineIntersectionToZPlane(eyePos, maxPointVs, tileFar);

	float3 minPointAABB = min(min(minPointNear, minPointFar), min(maxPointNear, maxPointFar));
	float3 maxPointAABB = max(max(minPointNear, minPointFar), max(maxPointNear, maxPointFar));
	
	ClusterList[tileIndex].minPoint = float4(minPointAABB, 1.0);
	ClusterList[tileIndex].maxPoint = float4(maxPointAABB, 1.0);
}