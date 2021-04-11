#pragma once
#ifndef _SHADER_H
#define _SHADER_H
#endif // !_SHADER_H


#include "../Common/CoreMini.h"
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
	Shader();
	Shader(const Shader& other);
	virtual ~Shader();

protected:
	//释放Shader
	virtual void ShutDown();

public:
	virtual void Apply() = 0;
	bool SetMatrix(const string& variableName, const CXMMATRIX& matrix);
	bool SetMatrixArrayElement(const string& variableName, const CXMMATRIX& matrix, int index);
	bool SetFloat(const string& variableName, float value);
	bool SetFloat2(const string& variableName, XMFLOAT2 value);
	bool SetFloat3(const string& variableName, XMFLOAT3 value);
	bool SetFloat3ArrayElement(const string& variableName, XMFLOAT3 value, int index);
	bool SetFloat4(const string& variableName, XMFLOAT4 value);
	bool SetTexture(const string& variableName, ID3D11ShaderResourceView* texture);
	bool SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler);

protected:
	bool ReflectShaderConstantBuffer(ID3D11ShaderReflection* reflection);
	bool ReflectShaderTexture(ID3D11ShaderReflection* shaderReflection);
	bool ReflectShaderSampler(ID3D11ShaderReflection* shaderReflection);
	bool CreateConstantBuffer();
	bool UpdateConstantBuffer();

protected:
	map<string, shared_ptr<ShaderConstantBuffer>> mapShaderContantBuffer;
	map<string, shared_ptr<ShaderVariable>> mapShaderVariable;
	map<string, shared_ptr<ShaderTexture>> mapShaderTexture;
	map<string, shared_ptr<ShaderSampler>> mapShaderSampler;
};

class VertexPixelShader : public Shader
{

public:
	VertexPixelShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	VertexPixelShader(const VertexPixelShader& other);
	~VertexPixelShader();

public:
	virtual void Apply() override;

private:
	bool Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	bool InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	void SetShaderParam();
	bool ReflectInputLayout(ID3D11ShaderReflection* vertexShaderReflection, ID3D10Blob* vertexShaderBlob);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
};