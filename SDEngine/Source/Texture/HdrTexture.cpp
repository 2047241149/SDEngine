#include "HdrTexture.h"
#include "stb_image.h"

HdrTexture::HdrTexture(char* TextureFilename):
	m_pSRV(nullptr)
{
	Init(TextureFilename);
}

HdrTexture::HdrTexture(const HdrTexture&other)
{

}

HdrTexture::~HdrTexture()
{
	ShutDown();
}

bool HdrTexture::Init(char* TextureFilename)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(TextureFilename, &width, &height, &nrComponents, 0);
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = width * 12;

	HR(g_pDevice->CreateTexture2D(&textureDesc, &initData, &texture));
	stbi_set_flip_vertically_on_load(false);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(g_pDevice->CreateShaderResourceView(texture, &srvDesc, &m_pSRV));
	ReleaseCOM(texture);
	return true;
}

void HdrTexture::ShutDown()
{
	ReleaseCOM(m_pSRV);
}

ID3D11ShaderResourceView* HdrTexture::GetTexture()
{
	return m_pSRV;
}