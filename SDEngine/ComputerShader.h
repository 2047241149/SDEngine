#pragma once
#ifndef _COMPUTER_SHADER_H
#define _COMPUTER_SHADER_H
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include "D3DClass.h"
#include<vector>
#include "Macro.h"

const int DATA_ARRAY_SIZE = 1024;

class ComputerShader
{

private:
	struct DataType
	{
		int i;
		float f;
	};

private:
	ID3D11ComputeShader* mComputerShader;
	ID3D11Buffer* mInputBufferA;
	ID3D11Buffer* mInputBufferB;
	ID3D11Buffer* mResultBuffer;

	ID3D11ShaderResourceView* mBufferASRV;
	ID3D11ShaderResourceView* mBufferBSRV;
	ID3D11UnorderedAccessView* mBufferResultUAV;

private:
	vector<DataType> mBufferDataA;
	vector<DataType> mBufferDataB;

public:
	ComputerShader(WCHAR* csFilenPath);
	ComputerShader(const ComputerShader& other);
	~ComputerShader();

	void RunComputer(ID3D11Buffer* pCBCS, void* pCDData,
		DWORD dwNumDataBytes, UINT dispatchX, UINT dispatchY, UINT dispatchZ);
	
private:
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename);
	bool virtual Initialize(WCHAR* csFilenPath);
	void CreateComputerShader(WCHAR* csFilenPath);
	void CreateBuffer();
	void Shutdown();
	void CreateStructBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut);
	void CreatBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	void CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAV);
	void CreateAndCopyToDebugBuf();
};

#endif // !_COMPUTER_SHADER_
