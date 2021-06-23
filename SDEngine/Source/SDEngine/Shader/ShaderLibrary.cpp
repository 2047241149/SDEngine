#include "ShaderLibrary.h"
#include "CascadeShadowMapManager.h"
#include "Log.h"

ShaderLibrary::ShaderLibrary()
{
}

ShaderLibrary::~ShaderLibrary()
{

}

ShaderLibrary::ShaderLibrary(const ShaderLibrary& other)
{

}

shared_ptr<ShaderLibrary> ShaderLibrary::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<ShaderLibrary>(new ShaderLibrary());
	}

	return single;
}

shared_ptr<VertexPixelShader> ShaderLibrary::GetShader(const string& shaderName)
{
	auto single = Get();
	if (single->shaderMap.find(shaderName) != single->shaderMap.end())
	{
		return single->shaderMap[shaderName];
	}

	return nullptr;
}
shared_ptr<VertexPixelShader> ShaderLibrary::LoadGetShader(const string& shaderFile)
{
	auto single = Get();
	if (single->shaderMap.find(shaderFile) != single->shaderMap.end())
	{
		return single->shaderMap[shaderFile];
	}

	string shaderName = GetShaderNameFromFile(shaderFile);
	shared_ptr<VertexPixelShader> shader = make_shared<VertexPixelShader>(shaderFile);
	if (nullptr == shader)
	{
		Log::Warn("LoadGetShader Shader Failed, shader file is %s\n", shaderFile);
		return nullptr;
	}

	single->shaderMap[shaderName] = shader;
	return shader;
}

void ShaderLibrary::LoadShader(const string& shaderFile)
{
	auto single = Get();
	if (single->shaderMap.find(shaderFile) == single->shaderMap.end())
	{
		string shaderName = GetShaderNameFromFile(shaderFile);
		shared_ptr<VertexPixelShader> shader = make_shared<VertexPixelShader>(shaderFile);
		if (nullptr == shader)
		{
			Log::Warn("LoadShader Shader Failed, shader file is %s\n", shaderFile);
		}

		single->shaderMap[shaderName] = shader;
	}
}

string ShaderLibrary::GetShaderNameFromFile(const string& shaderFile)
{
	int charArrowPos = (int)shaderFile.find_last_of('/');
	int charDotPos = (int)shaderFile.find_last_of('.');
	int length = charDotPos - charArrowPos - 1;
	string shaderName = shaderFile.substr(charArrowPos + 1, length);
	return shaderName;
}

shared_ptr<ShaderLibrary> ShaderLibrary::single = nullptr;