

RWStructuredBuffer<uint> globalIndexCount: register(u0);

[numthreads(1, 1, 1)]
void CS(
	uint3 groupId :  SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	globalIndexCount[0] = 0;
}