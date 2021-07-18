#include "Texture.h"
#include "SDEngine/Common/CommonFunction.h"

Texture::Texture(const string& textureFile):
	m_pSRV(nullptr)
{
	Init(textureFile);
}

Texture::~Texture()
{
	ShutDown();
}

bool Texture::Init(const string& textureFile)
{
	file = textureFile;
	wstring wFile = Str2Wstr(textureFile);
	HR(Resource::CreateShaderResourceViewFromFile(g_pDevice, wFile.c_str(), &m_pSRV));
	return true;
}

void Texture::ShutDown()
{
	ReleaseCOM(m_pSRV);
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_pSRV;
}