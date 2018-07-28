#include "Texture.h"

Texture::Texture(WCHAR* TextureFilename):
	m_pSRV(nullptr)
{
	Init(TextureFilename);
}

Texture::Texture(const Texture&other)
{

}

Texture::~Texture()
{
	ShutDown();
}

bool Texture::Init(WCHAR* TextureFilename)
{
	//ÖÃ¿ÕÖ¸Õë
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	HR(DXUTCreateShaderResourceViewFromFile(d3dDevice, TextureFilename, &m_pSRV));
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

ID3D11ShaderResourceView* Texture::LoadTexture(WCHAR* TextureFilename)
{
	return nullptr;
}
