

struct BufferType
{
	int i;
	int row;
	int column;
};

#define DATA_SIZE 32

StructuredBuffer<BufferType> Buffer0 :register(t0);
StructuredBuffer<BufferType> Buffer1 :register(t1);
RWStructuredBuffer<BufferType> BufferOut : register(u0);

[numthreads(1,1,1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	int index = DATA_SIZE * DTid.x + DTid.y;
	BufferOut[index].i = index;
	BufferOut[index].column = DTid.x;
	BufferOut[index].row = DTid.y;
}

