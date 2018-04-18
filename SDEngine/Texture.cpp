#include "Texture.h"

Texture::Texture(WCHAR* TextureFilename)
{
	Initilize(TextureFilename);
}

Texture::Texture(const Texture&other)
{

}

Texture::~Texture()
{
	ShutDown();
}

bool Texture::Initilize(WCHAR* TextureFilename)
{
	//ÖÃ¿ÕÖ¸Õë
	mTexture = NULL;
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	HR(DXUTCreateShaderResourceViewFromFile(d3dDevice, TextureFilename, &mTexture));
	return true;
}

void Texture::ShutDown()
{
	ReleaseCOM(mTexture);
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return mTexture;
}

