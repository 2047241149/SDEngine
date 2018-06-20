

struct BufferType
{
	int i;
	float f;
};

StructuredBuffer<BufferType> Buffer0 :register(t0);
StructuredBuffer<BufferType> Buffer1 :register(t1);
RWStructuredBuffer<BufferType> BufferOut : register(u0);

[numthreads(1,1,1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;
	BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;
}

