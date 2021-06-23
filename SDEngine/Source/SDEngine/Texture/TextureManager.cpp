#include "Texture.h"
#include "TextureManager.h"


TextureManager::TextureManager()
{
	Init();
}

TextureManager::TextureManager(const TextureManager& other)
{

}

TextureManager::~TextureManager()
{
	Release();
}

shared_ptr<TextureManager> TextureManager::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<TextureManager>(new TextureManager());
	}

	return single;
}

bool TextureManager::Init()
{
	whiteTexture = shared_ptr<Texture>(new Texture("Resource/white.png"));
	return true;
}


void TextureManager::Release()
{
}

ID3D11ShaderResourceView* TextureManager::LoadGetSRV(string fileName)
{
	if (fileName == "")
	{
		return nullptr;
	}

	ID3D11ShaderResourceView* memSRV = nullptr;

	if (srvMap.find(fileName) == srvMap.end())
	{
		Resource::CreateShaderResourceViewFromFile(g_pDevice, Str2Wstr(fileName).c_str(), &memSRV);
		
		if (UINT_PTR(memSRV) == 0xcccccccc)
		{
			memSRV = nullptr;
		}
		
		srvMap[fileName] = memSRV;
	}

	else
	{
		memSRV = srvMap[fileName];
	}

	return memSRV;
}

shared_ptr<TextureManager> TextureManager::single = nullptr;