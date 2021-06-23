#pragma once
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "Shader.h"

class Texture;
enum TextureSampler;

union MaterialVariableValue
{
	float floatValue;
	XMFLOAT2 float2Value;
	XMFLOAT3 float3Value;
	XMFLOAT4 float4Value;
	XMMATRIX matrix;
};

struct MaterialVariable
{
	ShaderVariableType type;
	MaterialVariableValue value;
};

class Material
{
private:
	//TODO: support StructBuffer And RWStructBuffer
	shared_ptr<VertexPixelShader> shader;
	map<string, shared_ptr<MaterialVariable>> materialValues;
	map<string, shared_ptr<Texture>> materialTextures;
	map<string, TextureSampler> materialTextureSamplers;

private:
	void ShutDown();
	void InitMaterialFromShader(shared_ptr<VertexPixelShader> shader);

public:
	Material(shared_ptr<VertexPixelShader> shader);
	Material(const Material& other);
	virtual ~Material();

public:
	void Apply();

public:
	//float
	bool SetFloat(const string& variableName, float value);
	bool SetFloat2(const string& variableName, XMFLOAT2 value);
	bool SetFloat3(const string& variableName, XMFLOAT3 value);
	bool SetFloat4(const string& variableName, XMFLOAT4 value);

	//matrix
	bool SetMatrix(const string& variableName, const CXMMATRIX& value);

	//texture
	bool SetTexture(const string& variableName, shared_ptr<Texture> texture);
	bool SetTextureSampler(const string& variableName, TextureSampler sampler);
};

#endif
