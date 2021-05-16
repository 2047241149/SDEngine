
#define GroundThreadSize 16

cbuffer CBCommon:register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float farPlane;
	float nearPlane;
	float4 tileSizes;
	float2 cluserFactor;
};

float DepthBufferConvertToLinear(float depth)
{
	float a = 1.0 / (nearPlane - farPlane);
	return (nearPlane*farPlane * a) / (depth + farPlane * a);
};

Texture2D<float4> DepthTex:register(t0);
RWStructuredBuffer<float> ClusterActiveList : register(u0);
SamplerState clampLinearSample:register(s0);

[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{

	float2 uv = float2(float(dispatchThreadId.x) / ScreenWidth, float(dispatchThreadId.y) / ScreenHeight);
	float2 screenPos = float2(uv.x * ScreenWidth, uv.y * ScreenHeight);
	float depth = DepthTex.SampleLevel(clampLinearSample, uv, 0).r;
	float viewZ = DepthBufferConvertToLinear(depth);
	uint clusterZ = uint(max(log2(viewZ) * cluserFactor.x + cluserFactor.y, 0.0));
	uint3 clusters = uint3(uint(screenPos.x / tileSizes.w), uint(screenPos.y / tileSizes.w), clusterZ);
	uint clusterIndex = clusters.x + clusters.y * (uint)tileSizes.x + clusters.z * (uint)tileSizes.x * (uint)tileSizes.y;
	ClusterActiveList[clusterIndex] = 1.0;
}