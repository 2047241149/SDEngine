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
	void* data;
	bool bNeedUpdated;
};

struct ShaderStructBuffer
{
	int size;
	int registerIndex;
	string name;
	void* data;
	bool bNeedUpdated;
	int structNum;
	ID3D11Buffer* buffer;
	ID3D11ShaderResourceView* srv;
};

struct ShaderRWStructBuffer
{
	int size;
	int registerIndex;
	string name;
	void* data;
	bool bNeedUpdated;
	int structNum;
	ID3D11Buffer* buffer;
	ID3D11ShaderResourceView* srv;
	ID3D11UnorderedAccessView* uav;
};

//TODO:部分代码重复,待重构
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

	//float
	bool SetFloat(const string& variableName, float value);
	bool SetFloat2(const string& variableName, XMFLOAT2 value);
	bool SetFloat3(const string& variableName, XMFLOAT3 value);
	bool SetFloat3ArrayElement(const string& variableName, XMFLOAT3 value, int index);
	bool SetFloat4(const string& variableName, XMFLOAT4 value);

	//uint
	bool SetUint4(const string& variableName, UINT8 value[4]);
	void SetRWStructBufferInData(const string& bufferName, void* data, int num);
	bool SetRWStructBuffer(const string& bufferName, ID3D11UnorderedAccessView* uav);
	virtual bool SetTexture(const string& variableName, ID3D11ShaderResourceView* texture) = 0;
	virtual bool SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler) = 0;
	ID3D11ShaderResourceView* GetSrvOfUav(const string& bufferName);
	ID3D11UnorderedAccessView* GetUav(const string& bufferName);

protected:
	bool ReflectShaderConstantBuffer(ID3D11ShaderReflection* reflection);
	bool ReflectShaderTexture(ID3D11ShaderReflection* shaderReflection);
	bool ReflectShaderSampler(ID3D11ShaderReflection* shaderReflection);
	bool ReflectShaderRWStructBuffer(ID3D11ShaderReflection* reflection);
	bool CreateConstantBuffer();

	// TODO: refactor UpdateConstantBuffer and UpdateRWStructBuffer, too much reappearing code
	virtual bool UpdateConstantBuffer() = 0;
	virtual bool UpdateRWStructBuffer() = 0;
	virtual void SetShaderParam() = 0;

protected:
	map<string, shared_ptr<ShaderConstantBuffer>> mapShaderContantBuffer;
	map<string, shared_ptr<ShaderVariable>> mapShaderVariable;
	map<string, shared_ptr<ShaderTexture>> mapShaderTexture;
	map<string, shared_ptr<ShaderSampler>> mapShaderSampler;

	//RWStructBuffer exit in PixelShader And ComputeShader
	map<string, shared_ptr<ShaderRWStructBuffer>> mapShaderRWStructBuffer;
};

class VertexPixelShader : public Shader
{

public:
	VertexPixelShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	VertexPixelShader(const VertexPixelShader& other);
	~VertexPixelShader();

public:
	virtual void Apply() override;
	virtual bool SetTexture(const string& variableName, ID3D11ShaderResourceView* texture) override;
	virtual bool SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler) override;

private:
	bool Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	bool InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	virtual void SetShaderParam() override;
	bool ReflectInputLayout(ID3D11ShaderReflection* vertexShaderReflection, ID3D10Blob* vertexShaderBlob);
	virtual bool UpdateConstantBuffer() override;
	virtual bool UpdateRWStructBuffer() override;

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
};

class ComputeShader : public Shader
{
public:
	ComputeShader(WCHAR* csFilenPath);
	ComputeShader(const ComputeShader& other);
	~ComputeShader();

public:
	virtual bool SetTexture(const string& variableName, ID3D11ShaderResourceView* texture) override;
	bool SetRWTexture(const string& variableName, ID3D11UnorderedAccessView* texture);
	virtual bool SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler) override;
	void SetStructBuffer(const string& bufferName, void* data, int num);
	void Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);
	ID3D11Buffer* GetBufferOfUav(const string& variableName);

private:
	bool ReflectShaderStructBuffer(ID3D11ShaderReflection* reflection);
	bool ReflectShaderUAVTexture(ID3D11ShaderReflection* reflection);
	virtual bool UpdateConstantBuffer() override;
	virtual bool UpdateRWStructBuffer() override;
	bool UpdateSutrctBuffer();
	void SetShaderParam();
	bool InitShader(WCHAR* csFilenPath);
	virtual void Apply() override;

private:
	ID3D11ComputeShader* computeShader;

private:
	map<string, shared_ptr<ShaderTexture>> mapRWShaderTexture;
	map<string, shared_ptr<ShaderStructBuffer>> mapShaderStructBuffer;
};