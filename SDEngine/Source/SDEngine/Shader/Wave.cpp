#include "Wave.h"

Wave::Wave(int waveWidth, int waveHeight, float waveGridSize,int uvTile):
	mWaveGridSize(waveGridSize),mUVTile(uvTile)
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//waveWidth waveHeight
	mWaveWidth = (waveWidth % 2) ? (waveWidth + 1) : (waveWidth);
	mWaveHeight = (waveHeight % 2) ? (waveHeight + 1) : (waveHeight);
	
	//TriangleStrip
	mWaveVertexData.resize((mWaveWidth + 1)* (mWaveHeight + 1));
	mWaveIndexData.resize(mWaveWidth * mWaveHeight * 6);
	Init();
	
}

Wave::Wave(int waveWidth, int waveHeight, float waveGridSize)
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//waveWidth waveHeight
	mWaveWidth = (waveWidth % 2) ? (waveWidth + 1) : (waveWidth);
	mWaveHeight = (waveHeight % 2) ? (waveHeight + 1) : (waveHeight);

	//TriangleStrip
	mWaveVertexData.resize((mWaveWidth + 1)* (mWaveHeight + 1));
	mWaveIndexData.resize(mWaveWidth* mWaveHeight * 6);
	Init();
}

Wave::~Wave()
{
}

void Wave::Init()
{

	//VertexBuffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPCNTT) * (UINT)mWaveVertexData.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &mWaveVertexData[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);


	int waveIndex = 0;

	//IndexData
	
	for (int heightIndex = 0; heightIndex < mWaveHeight; ++heightIndex)
	{
		for (int widthIndex = 0; widthIndex < mWaveWidth; ++widthIndex)
		{
			//vertex1
			WORD vertexIndex = heightIndex * (mWaveWidth + 1) + widthIndex;	
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex3
			vertexIndex = (heightIndex + 1) * (mWaveWidth + 1) + widthIndex + 1;
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex2
			vertexIndex = heightIndex * (mWaveWidth + 1) + widthIndex + 1;
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex1
			vertexIndex = heightIndex * (mWaveWidth + 1) + widthIndex;
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex4
			vertexIndex = (heightIndex + 1) * (mWaveWidth + 1) + widthIndex;
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex3
			vertexIndex = (heightIndex + 1) * (mWaveWidth + 1) + widthIndex + 1;
			mWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;
		}
	}


	//IndexBuffer
	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * (UINT)mWaveIndexData.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &mWaveIndexData[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	g_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
}

void Wave::Render()
{
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//VertexBuffetr
	UINT stride = sizeof(VertexPCNTT);
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	g_pDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pDeviceContext->DrawIndexed((UINT)mWaveIndexData.size(), 0, 0);
}

void Wave::CalculateVertexNormal()
{

	for (int row = 0; row < mWaveHeight + 1; ++row)
	{
		for (int column = 0; column < mWaveWidth + 1; ++column)
		{
			int index = row * (mWaveWidth + 1) + column;
			XMFLOAT3 vertex1 = mWaveVertexData[index].pos;

			if ((0 == row && 0 == column))
			{
				XMFLOAT3 vertex2 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1].pos;
				mWaveVertexData[index].normal = CrossNormal(vertex1, vertex2, vertex3);
			}

			else if (0 == row && column == mWaveWidth)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column].pos;
				mWaveVertexData[index].normal = CrossNormal(vertex1, vertex2, vertex3);
			}
			else if (mWaveHeight == row && 0 == column)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				mWaveVertexData[index].normal = CrossNormal(vertex1, vertex2, vertex3);
			}
			else if (mWaveHeight == row && column == mWaveWidth)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1].pos;
				mWaveVertexData[index].normal = CrossNormal(vertex1, vertex2, vertex3);
			}
			else if (0 == column)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1].pos;
				XMFLOAT3 vertex4 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
				XMFLOAT3 normal2 = CrossNormal(vertex1, vertex3, vertex4);
				mWaveVertexData[index].normal =
					XMFLOAT3((normal1.x + normal2.x) / 2.0f, (normal1.y + normal2.y) / 2.0f, (normal1.z + normal2.z) / 2.0f);
			}
			else if (column == mWaveWidth)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1].pos;
				XMFLOAT3 vertex4 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
				XMFLOAT3 normal2 = CrossNormal(vertex1, vertex3, vertex4);
				mWaveVertexData[index].normal =
					XMFLOAT3((normal1.x + normal2.x) / 2.0f, (normal1.y + normal2.y) / 2.0f, (normal1.z + normal2.z) / 2.0f);
			}
			else if (0 == row)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[row  * (mWaveWidth + 1) + column - 1].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex4 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1].pos;
				XMFLOAT3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
				XMFLOAT3 normal2 = CrossNormal(vertex1, vertex3, vertex4);
				mWaveVertexData[index].normal =
					XMFLOAT3((normal1.x + normal2.x) / 2.0f, (normal1.y + normal2.y) / 2.0f, (normal1.z + normal2.z) / 2.0f);
			}
			else if (mWaveHeight == row)
			{
				XMFLOAT3 vertex2 = mWaveVertexData[row  * (mWaveWidth + 1) + column + 1].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex4 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1].pos;
				XMFLOAT3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
				XMFLOAT3 normal2 = CrossNormal(vertex1, vertex3, vertex4);

				mWaveVertexData[index].normal =
					XMFLOAT3((normal1.x + normal2.x) / 2.0f, 
					(normal1.y + normal2.y) / 2.0f, (normal1.z + normal2.z) / 2.0f);
			}
			else
			{
				XMFLOAT3 vertex2 = mWaveVertexData[(row + 1)  * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1].pos;
				XMFLOAT3 vertex4 = mWaveVertexData[(row - 1) * (mWaveWidth + 1) + column].pos;
				XMFLOAT3 vertex5 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1].pos;
				XMFLOAT3 normal1 = CrossNormal(vertex1, vertex2, vertex3);
				XMFLOAT3 normal2 = CrossNormal(vertex1, vertex3, vertex4);
				XMFLOAT3 normal3 = CrossNormal(vertex1, vertex4, vertex5);
				XMFLOAT3 normal4 = CrossNormal(vertex1, vertex5, vertex2);
				float normalX = (normal1.x + normal2.x + normal3.x + normal4.x) / 4.0f;
				float normalY = (normal1.y + normal2.y + normal3.y + normal4.y) / 4.0f;
				float normalZ = (normal1.z + normal2.z + normal3.z + normal4.z) / 4.0f;
				mWaveVertexData[index].normal = XMFLOAT3(normalX, normalY, normalZ);
			}

			NormalizeFloat3(mWaveVertexData[index].normal);
		}

	}
}

void Wave::CalculateVertexPos(float time)
{
	UINT index = 0;

	for (int posZ = -mWaveHeight / 2; posZ <= mWaveHeight / 2; ++posZ)
	{
		for (int posX = -mWaveWidth / 2; posX <= mWaveWidth / 2; ++posX)
		{
			float positionX = (float)posX * mWaveGridSize;
			float positionZ = (float)posZ * mWaveGridSize;

			//pos
			mWaveVertexData[index].pos.x = positionX;
			mWaveVertexData[index].pos.z = positionZ;
			mWaveVertexData[index].pos.y = (float)GetWaveVertexHeight((int)positionX, (int)positionZ, time);
			++index;
		}
	}
}

XMFLOAT3 Wave::CrossNormal(const XMFLOAT3& vertex1, const XMFLOAT3& vertex2, const XMFLOAT3& vertex3)
{
	XMFLOAT3 normal;
	XMVECTOR vec1 = XMVectorSet(vertex2.x - vertex1.x, vertex2.y - vertex1.y, vertex2.z - vertex1.z, 0.0f);
	XMVECTOR vec2 = XMVectorSet(vertex3.x - vertex1.x, vertex3.y - vertex1.y, vertex3.z - vertex1.z, 0.0f);
	XMVECTOR normalVec = XMVector3Cross(vec1, vec2);
	XMStoreFloat3(&normal, normalVec);

	return normal;
}

void Wave::NormalizeFloat3(XMFLOAT3& normal)
{
	float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;
}

void Wave::SetWaveParam(int uvTile)
{
	mUVTile = uvTile;
}

void Wave::CalculateVertexUV()
{
	for (int row = 0; row < mWaveHeight + 1; ++row)
	{
		for (int column = 0; column < mWaveWidth + 1; ++column)
		{
			int index = row * (mWaveWidth + 1) + column;
			float u = ((float)(column) / (float)mWaveHeight);
			float v = ((float)(row) / (float)mWaveWidth);
			mWaveVertexData[index].uv.x = u;
			mWaveVertexData[index].uv.y = v;
		}
	}

}

void Wave::CalculateVertexTangent()
{
	for (int row = 0; row < mWaveHeight + 1; ++row)
	{
		for (int column = 0; column < mWaveWidth + 1; ++column)
		{
			int index = row * (mWaveWidth + 1) + column;
			VertexPCNTT vertex1;
			VertexPCNTT vertex2;
			VertexPCNTT vertex3;
			
			if (row >= 0 && row <= mWaveHeight - 1 && column >= 0 && column <= mWaveWidth - 1)
			{
				vertex1 = mWaveVertexData[index];
				vertex2 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column];
				vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column + 1];

			}
			else if (row != mWaveHeight &&column == mWaveWidth)
			{
				vertex1 = mWaveVertexData[index];
				vertex2 = mWaveVertexData[(row + 1) * (mWaveWidth + 1) + column];
				vertex3 = mWaveVertexData[row * (mWaveWidth + 1) + column - 1];
			}
			else
			{
				mWaveVertexData[index].tangent = mWaveVertexData[index - mWaveWidth - 1].tangent;
				continue;
			}
		
			float Edge1[3], Edge2[3];
			float TexEdge1[2], TexEdge2[2];
			Edge1[0] = vertex2.pos.x - vertex1.pos.x; //E0X  
			Edge1[1] = vertex2.pos.y - vertex1.pos.y; //E0Y  
			Edge1[2] = vertex2.pos.z - vertex1.pos.z; //E0Z  

			Edge2[0] = vertex3.pos.x - vertex1.pos.x; //E1X  
			Edge2[1] = vertex3.pos.y - vertex1.pos.y; //E1Y  
			Edge2[2] = vertex3.pos.z - vertex1.pos.z; //E1Z  

			TexEdge1[0] = vertex2.uv.x - vertex1.uv.x; //U0  
			TexEdge1[1] = vertex2.uv.y - vertex1.uv.y; //V0  

			TexEdge2[0] = vertex3.uv.x - vertex1.uv.x; //U1  
			TexEdge2[1] = vertex3.uv.y - vertex1.uv.y; //V1  

			float den = 1.0f / (TexEdge1[0] * TexEdge2[1] - TexEdge1[1] * TexEdge2[0]);
			XMFLOAT3 tangent;
			tangent.x = den*(TexEdge2[1] * Edge1[0] - TexEdge1[1] * Edge2[0]);
			tangent.y = den*(TexEdge2[1] * Edge1[1] - TexEdge1[1] * Edge2[1]);
			tangent.z = den*(TexEdge2[1] * Edge1[2] - TexEdge1[1] * Edge2[2]);
			NormalizeFloat3(tangent);

			XMFLOAT3& normal = mWaveVertexData[index].normal;
			float dot = Dot(normal, tangent);
			tangent.x = tangent.x - dot * normal.x;
			tangent.y = tangent.y - dot * normal.y;
			tangent.z = tangent.z - dot * normal.z;
			NormalizeFloat3(tangent);	
			mWaveVertexData[index].tangent = tangent;
		}
	}
}

void Wave::UpdateWaveData(float time)
{
	CalculateVertexPos(time);
	CalculateVertexNormal();
	CalculateVertexUV();
	CalculateVertexTangent();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_pDeviceContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VertexPCNTT* verticesPtr;
	verticesPtr = (VertexPCNTT*)mappedResource.pData;
	memcpy(verticesPtr, (void*)&mWaveVertexData[0], (sizeof(VertexPCNTT) * mWaveVertexData.size()));
	g_pDeviceContext->Unmap(mVertexBuffer, 0);
}

float Wave::Dot(const XMFLOAT3& vec1, const XMFLOAT3& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

XMMATRIX Wave::GetWorldMatrix()
{
	return XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
}