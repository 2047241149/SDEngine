#include "Material.h"
#include "Log.h"
#include "Texture/TextureManager.h"
#include "Texture/TextureSamplerManager.h"
#include "Texture/Texture.h"

Material::Material(shared_ptr<VertexPixelShader> shader)
{
	InitMaterialFromShader(shader);
}

Material::Material(const Material& other)
{
}

Material::~Material()
{
	ShutDown();
}


void Material::ShutDown()
{
}

void Material::InitMaterialFromShader(shared_ptr<VertexPixelShader> shader)
{
	this->shader = shader;

	if (shader == nullptr)
	{
		Log::Error("InitMaterialFromShader Error: shader is nullptr");
		return;
	}

	//construct material variable
	for (auto& it : shader->mapShaderVariable)
	{
		shared_ptr<MaterialVariable> variable = make_shared<MaterialVariable>();
		variable->type = it.second->variableType;

		switch (variable->type)
		{
			case ShaderVariableType::SHADER_FLOAT:
				variable->value.floatValue = 1.0f;
				break;

			case ShaderVariableType::SHADER_FLOAT2:
				variable->value.float2Value = XMFLOAT2(1.0f, 1.0f);
				break;

			case ShaderVariableType::SHADER_FLOAT3:
				variable->value.float3Value = XMFLOAT3(1.0f, 1.0f, 1.0f);
				break;

			case ShaderVariableType::SHADER_FLOAT4:
				variable->value.float4Value = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				break;

			case ShaderVariableType::SHADER_MATRIX:
				variable->value.matrix = XMMatrixIdentity();
				break;

			default:
				break;
		}

		materialValues[it.first] = variable;
	}

	//construct material texture
	for (auto& it : shader->mapShaderTexture)
	{
		materialTextures[it.first] = GWhiteTexture;
	}

	//construct material texture sampler
	for (auto& it : shader->mapShaderSampler)
	{
		materialTextureSamplers[it.first] = TextureSampler::BilinearFliterClamp;
	}
}

bool Material::SetFloat(const string& variableName, float value)
{
	if (materialValues.find(variableName) != materialValues.end() && 
		materialValues[variableName]->type == ShaderVariableType::SHADER_FLOAT)
	{
		materialValues[variableName]->value.floatValue = value;
		return true;
	}

	Log::Warn("Material SetFloat Failed, variableName = {0}\n", variableName);
	return false;
}

bool Material::SetFloat2(const string& variableName, XMFLOAT2 value)
{
	if (materialValues.find(variableName) != materialValues.end() && 
		materialValues[variableName]->type == ShaderVariableType::SHADER_FLOAT2)
	{
		materialValues[variableName]->value.float2Value = value;
		return true;
	}

	Log::Warn("Material SetFloat2 Failed, variableName = {0}\n", variableName);
	return false;
}

bool Material::SetFloat3(const string& variableName, XMFLOAT3 value)
{
	if (materialValues.find(variableName) != materialValues.end() &&
		materialValues[variableName]->type == ShaderVariableType::SHADER_FLOAT3)
	{
		materialValues[variableName]->value.float3Value = value;
		return true;
	}

	Log::Warn("Material SetFloat3 Failed, variableName = {0}\n", variableName);
	return false;
}

bool Material::SetFloat4(const string& variableName, XMFLOAT4 value)
{
	if (materialValues.find(variableName) != materialValues.end() &&
		materialValues[variableName]->type == ShaderVariableType::SHADER_FLOAT4)
	{
		materialValues[variableName]->value.float4Value = value;
		return true;
	}

	Log::Warn("Material SetFloat4 Failed, variableName = {0}\n", variableName);
	return false;
}

bool Material::SetMatrix(const string& variableName, const CXMMATRIX& value)
{
	if (materialValues.find(variableName) != materialValues.end() &&
		materialValues[variableName]->type == ShaderVariableType::SHADER_MATRIX)
	{
		materialValues[variableName]->value.matrix = value;
		return true;
	}

	Log::Warn("Material SetMatrix Failed, variableName = {0}\n", variableName);
	return false;
}

bool Material::SetTexture(const string& variableName, shared_ptr<Texture> texture)
{
	if (texture == nullptr)
	{
		Log::Warn("Material SetTexture Failed, texture is nullptr, variableName = {0}\n", variableName);
		return false;
	}

	if (materialTextures.find(variableName) != materialTextures.end())
	{
		materialTextures[variableName] = texture;
		return true;
	}

	Log::Warn("Material SetTexture Failed, variableName = {0}\n", variableName);
	return false;
}


bool Material::SetTextureSampler(const string& variableName, TextureSampler sampler)
{
	if (materialTextureSamplers.find(variableName) != materialTextureSamplers.end())
	{
		materialTextureSamplers[variableName] = sampler;
		return true;
	}

	Log::Warn("Material SetTextureSampler Failed, variableName = {0}\n", variableName);
	return false;
}

void Material::Apply()
{
	if (shader == nullptr)
	{
		Log::Error("Material Apply Failed Error: shader is nullptr");
		return;
	}

	//setup shader variable
	for (auto& it : materialValues)
	{
		switch (it.second->type)
		{
		case ShaderVariableType::SHADER_FLOAT:
			shader->SetFloat(it.first, it.second->value.floatValue);
			break;

		case ShaderVariableType::SHADER_FLOAT2:
			shader->SetFloat2(it.first, it.second->value.float2Value);
			break;

		case ShaderVariableType::SHADER_FLOAT3:
			shader->SetFloat3(it.first, it.second->value.float3Value);
			break;

		case ShaderVariableType::SHADER_FLOAT4:
			shader->SetFloat4(it.first, it.second->value.float4Value);
			break;

		case ShaderVariableType::SHADER_MATRIX:
			shader->SetMatrix(it.first, it.second->value.matrix);
			break;

		default:
			break;
		}
	}

	//setup shader texture
	for (auto& it : materialTextures)
	{
		shader->SetTexture(it.first, it.second->GetTexture());
	}

	//setup shader texture
	for (auto& it : materialTextureSamplers)
	{
		shader->SetTextureSampler(it.first, GTextureSamplerManager->GetTextureSampler(it.second));
	}

	shader->Apply();
}