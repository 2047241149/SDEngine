

/*
* ThreadGround维度:(fGroundCountX, fGroundCountY, 1)
* 每个线程团的Thread维度为(GroundThreadSize, GroundThreadSize, 1)
* SV_DispatchThreadID范围:(0,0,0) - (fGroundCountX * GroundThreadSize, fGroundCountY * GroundThreadSize,0)
*/


#define GroundThreadSize 16
static const float  XM_PI = 3.141592f;
static const int WaveParamArraySize = 3;

//顶点格式
struct VertexPCNTT
{
	float3 pos;
	float3 color;
	float3 normal;
	float3 tangent;
	float2 uv;
};

struct GerstnerParam
{
	float fAmplitude;
	float fSpeed;
	float fWaveLength;
	float fSteepness;
	float2 direction;
	float2 pad;
};


//每帧都不改变的常量缓存
cbuffer CBGerstnerWaveNoUpdate:register(b0)
{
	GerstnerParam gerstnerData[WaveParamArraySize];
	float fWaveGridSize;
	float fGroundCountX; //X方向有几个线程团   
	float fGroundCountY; //Y方向有几个线程团
	float fUVTile;
	float fUVMoveSpeed;
	float3 pad1;
};

//每帧更新的常量缓存
cbuffer CBGerstnerWaveUpdate:register(b1)
{
	float fCurrentTime;
	float3 pad;
}


RWStructuredBuffer<VertexPCNTT> WaveData : register(u0);

float GetGerstnerHeight(float fPosX, float fPosZ, float time)
{
	float fHeight = 0.0f;

	for (int iParamIndex = 0; iParamIndex < WaveParamArraySize; ++iParamIndex)
	{
		GerstnerParam gerstnerParam = gerstnerData[iParamIndex];

		float fDdotXZ = gerstnerParam.direction.x * fPosX + gerstnerParam.direction.y * fPosZ;

		//频率
		float w = 2.0f * XM_PI / gerstnerParam.fWaveLength;

		//相位差常量
		float phaseConstant = gerstnerParam.fSpeed * w;

		float param = fDdotXZ * w + time * phaseConstant;
		//posZ
		fHeight += (float)gerstnerParam.fAmplitude * sin(param);

	}

	return fHeight;
}


float GetPosZOffset(float x, float z, float time)
{
	float fPosZOffset = 0.0f;
	for (int iParamIndex = 0; iParamIndex < WaveParamArraySize; ++iParamIndex)
	{
		GerstnerParam gerstnerParam = gerstnerData[iParamIndex];

		float fDdotXZ = gerstnerParam.direction.x * x + gerstnerParam.direction.y * z;

		//频率
		float w = 2.0f * XM_PI / gerstnerParam.fWaveLength;

		//相位差常量
		float phaseConstant = gerstnerParam.fSpeed * w;

		fPosZOffset += (float)gerstnerParam.fAmplitude * gerstnerParam.fSteepness *
			gerstnerParam.direction.y * cos(fDdotXZ * w + time * phaseConstant);
	}

	return fPosZOffset;
}


float GetPosXOffset(float x, float z, float time)
{
	float fPosXOffset = 0.0f;

	for (int iParamIndex = 0; iParamIndex < WaveParamArraySize; ++iParamIndex)
	{
		GerstnerParam gerstnerParam = gerstnerData[iParamIndex];

		float fDdotXZ = gerstnerParam.direction.x * x + gerstnerParam.direction.y * z;

		//频率
		float w = 2.0f * XM_PI / gerstnerParam.fWaveLength;

		//相位差常量
		float phaseConstant = gerstnerParam.fSpeed * w;

		fPosXOffset += (float)gerstnerParam.fAmplitude * gerstnerParam.fSteepness *
			gerstnerParam.direction.x * cos(fDdotXZ * w + time * phaseConstant);
	}

	return fPosXOffset;
}



[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void WaveVertexPosUVColor_CS(uint3 DTid : SV_DispatchThreadID)
{
	int index = DTid.x + DTid.y * fGroundCountX * GroundThreadSize;
	float posX = (float)DTid.x * fWaveGridSize;
	float posZ = (float)DTid.y * fWaveGridSize;
	float posXOffset = GetPosXOffset(posX, posZ, fCurrentTime);
	float posZOffset = GetPosZOffset(posX, posZ, fCurrentTime);
	float posY = GetGerstnerHeight(posX, posZ, fCurrentTime);
	WaveData[index].pos = float3(posX + posXOffset, posY, posZ + posZOffset);
	WaveData[index].color = float3(1.0f, 1.0f, 1.0f);
	WaveData[index].uv.x = (DTid.x + fUVMoveSpeed * fUVTile * fCurrentTime) / fUVTile;
	WaveData[index].uv.y = (DTid.y + fUVMoveSpeed * fUVTile * fCurrentTime) / fUVTile;
	WaveData[index].normal = float3(0.0f, 0.0f, 0.0f);
	WaveData[index].tangent = float3(0.0f, 0.0f, 0.0f);
}



//----------------------------------------------------------------------------

float3 CrossNormal(float3 vertex1, float3 vertex2, float3 vertex3)
{
	float3 vec1 = float3(vertex2.x - vertex1.x, vertex2.y - vertex1.y, vertex2.z - vertex1.z);
	float3 vec2 = float3(vertex3.x - vertex1.x, vertex3.y - vertex1.y, vertex3.z - vertex1.z);
	float3 normal = cross(vec1, vec2);
	return normal;
}

void CalculateNormal(uint3 DTid)
{
	int width = fGroundCountX * GroundThreadSize;
	int index = DTid.x + DTid.y * width;

	//大致忽略网格边缘问题，否则都判断很多边界问题
	//GPU的StructBuffer具备溢出写入无效和溢出读为0的性质
	float3 vertex1 = WaveData[index].pos;
	float3 vertex2 = WaveData[(DTid.y + 1)  * width + DTid.x].pos;
	float3 vertex3 = WaveData[DTid.y * width + DTid.x + 1].pos;
	float3 vertex4 = WaveData[(DTid.y - 1) * width + DTid.x].pos;
	float3 vertex5 = WaveData[DTid.y * width + DTid.x - 1].pos;
	float3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
	float3 normal2 = CrossNormal(vertex1, vertex3, vertex4);
	float3 normal3 = CrossNormal(vertex1, vertex4, vertex5);
	float3 normal4 = CrossNormal(vertex1, vertex5, vertex2);
	float normalX = (normal1.x + normal2.x + normal3.x + normal4.x) / 4.0f;
	float normalY = (normal1.y + normal2.y + normal3.y + normal4.y) / 4.0f;
	float normalZ = (normal1.z + normal2.z + normal3.z + normal4.z) / 4.0f;
	float3 normal = float3(normalX, normalY, normalZ);
	WaveData[index].normal = normalize(normal);
}


void CalculateTangent(uint3 DTid)
{
	int width = fGroundCountX * GroundThreadSize;
	int index = DTid.x + DTid.y * width;
	VertexPCNTT vertex1;
	VertexPCNTT vertex2;
	VertexPCNTT vertex3;

	vertex1 = WaveData[index];
	vertex2 = WaveData[(DTid.y + 1) * width + DTid.x];
	vertex3 = WaveData[DTid.y * width + DTid.x + 1];

	float Edge1[3], Edge2[3];
	float TexEdge1[2], TexEdge2[2];

	//计算面的两个向量  
	//边向量1  
	Edge1[0] = vertex2.pos.x - vertex1.pos.x; //E0X  
	Edge1[1] = vertex2.pos.y - vertex1.pos.y; //E0Y  
	Edge1[2] = vertex2.pos.z - vertex1.pos.z; //E0Z  

	//边向量2  
	Edge2[0] = vertex3.pos.x - vertex1.pos.x; //E1X  
	Edge2[1] = vertex3.pos.y - vertex1.pos.y; //E1Y  
	Edge2[2] = vertex3.pos.z - vertex1.pos.z; //E1Z  

	//纹理边向量1  
	TexEdge1[0] = vertex2.uv.x - vertex1.uv.x; //U0  
	TexEdge1[1] = vertex2.uv.y - vertex1.uv.y; //V0  

	//纹理边向量2  
	TexEdge2[0] = vertex3.uv.x - vertex1.uv.x; //U1  
	TexEdge2[1] = vertex3.uv.y - vertex1.uv.y; //V1  

	//求出TB在模型空间坐标的方程系数  
	float den = 1.0f / (TexEdge1[0] * TexEdge2[1] - TexEdge1[1] * TexEdge2[0]);

	//求出Tangent  
	float3 tangent;
	tangent.x = den * (TexEdge2[1] * Edge1[0] - TexEdge1[1] * Edge2[0]);
	tangent.y = den * (TexEdge2[1] * Edge1[1] - TexEdge1[1] * Edge2[1]);
	tangent.z = den * (TexEdge2[1] * Edge1[2] - TexEdge1[1] * Edge2[2]);
	tangent = -normalize(tangent);

	//求出垂直于Nomral的Tangent
	float3 normal = WaveData[index].normal;
	float dotA = dot(normal, tangent);
	tangent = normalize(tangent - dotA * normal);

	WaveData[index].tangent = tangent;

}

[numthreads(GroundThreadSize, GroundThreadSize, 1)]
void WaveNormalTangent_CS(uint3 DTid : SV_DispatchThreadID)
{
	CalculateNormal(DTid);
	CalculateTangent(DTid);
}