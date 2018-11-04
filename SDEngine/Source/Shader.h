#pragma once
#ifndef _SHADER_H
#define _SHADER_H
#endif // !_SHADER_H


#include "CoreMini.h"
#include <d3dcompiler.h>


enum ShaderVariableType
{
	SHADER_MATRIX,
	SHADER_FLOAT,
	SHADER_FLOAT2,
	SHADER_FLOAT3,
	SHADER_FLOAT4
};

struct ShaderVariable
{
	int startOffset;
	int size;
	string name;
	string constantBufferName;
	ShaderVariableType variableType;
	//当variablePre与variableCurrent指向的值一致时，将不进行常量缓存更新
	void* variablePre; 	//variablePre 指向之前变量的值
	void* variableCurrent; 	//variableCurrent 指向目前变量的值
};

struct ShaderTexture
{
	string name;
	int bindPoint;
};


struct ShaderSampler
{
	string name;
	int bindPoint;
};

struct ShaderConstantBuffer
{
	int size;
	int registerIndex;
	string name;
	ID3D11Buffer* constantBuffer;
	vector<string> vecShaderVariableName;
	bool bNeedUpdated;
};


class Shader
{

public:
	Shader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	Shader(const Shader& other);
	virtual ~Shader();

public:
	virtual bool  Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader
	virtual bool  InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);

protected:
	//释放Shader
	virtual void ShutDown();
	virtual void SetShaderState();

public:
	void Apply();
	bool SetMatrix(const string& variableName, const CXMMATRIX& matrix);
	bool SetMatrixArrayElement(const string& variableName, const CXMMATRIX& matrix, int index);
	bool SetFloat(const string& variableName, float value);
	bool SetFloat2(const string& variableName, XMFLOAT2 value);
	bool SetFloat3(const string& variableName, XMFLOAT3 value);
	bool SetFloat4(const string& variableName, XMFLOAT4 value);
	bool SetTexture(const string& variableName, ID3D11ShaderResourceView* texture);
	bool SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler);

private:
	bool ReflectShaderConstantBuffer(ID3D11ShaderReflection* reflection);
	bool ReflectShaderTexture(ID3D11ShaderReflection* shaderReflection);
	bool ReflectShaderSampler(ID3D11ShaderReflection* shaderReflection);
	bool ReflectInputLayout(ID3D11ShaderReflection* vertexShaderReflection, ID3D10Blob* vertexShaderBlob);
	bool CreateConstantBuffer();
	bool UpdateConstantBuffer();

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState *m_pWrapLinearSampler;
	ID3D11SamplerState *m_pClampPointSampler;
	map<string, shared_ptr<ShaderConstantBuffer>> mapShaderContantBuffer;
	map<string, shared_ptr<ShaderVariable>> mapShaderVariable;
	map<string, shared_ptr<ShaderTexture>> mapShaderTexture;
	map<string, shared_ptr<ShaderSampler>> mapShaderSampler;
};
