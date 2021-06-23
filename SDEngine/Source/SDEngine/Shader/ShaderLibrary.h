#pragma once
#ifndef _SHADER_LIBRARY_H
#define _SHADER_LIBRARY_H
#include "Shader.h"
#include <memory>
#include "SDEngine/Common/Macro.h"
using namespace std;


class ShaderLibrary
{
private:
	static shared_ptr<ShaderLibrary> single;
	
private:
	map<string, shared_ptr<VertexPixelShader>> shaderMap;

public:
	ShaderLibrary();
	~ShaderLibrary();
	ShaderLibrary(const ShaderLibrary&);

	static shared_ptr<VertexPixelShader> GetShader(const string& shaderName);
	static shared_ptr<VertexPixelShader> LoadGetShader(const string& shaderFile);
	static void LoadShader(const string& shaderFile);
	static string GetShaderNameFromFile(const string& shaderFile);
	static shared_ptr<ShaderLibrary> Get();
};
#endif 