#include "NoiseTexture.h"
#include <time.h>
NoiseTexture::NoiseTexture(int textureWidth, int textureHeight)
{
	Init(textureWidth, textureHeight);
}

NoiseTexture::NoiseTexture(const NoiseTexture& other)
{

}

NoiseTexture::~NoiseTexture()
{

}

bool NoiseTexture::Init(int textureWidth, int textureHeight)
{
	if (0 == textureWidth || 0 == textureHeight)
		return false;

	srand((unsigned)time(NULL));
	XMFLOAT3* color = new XMFLOAT3[textureWidth * textureHeight];
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_SUBRESOURCE_DATA pInitialData = { 0 };
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	
	for (int x = 0; x < textureWidth; ++x)
	{
		for (int y = 0; y < textureHeight; ++y)
		{
			//-1.0 - 1.0  -1.0 - 1.0  0.0
			color[y * textureWidth + x] = XMFLOAT3(((float)rand() / (float)RAND_MAX) * 2.0f -  1.0f, ((float)rand() / (float)RAND_MAX)* 2.0f - 1.0f, 0.0f);
		}
	}

	ID3D11Texture2D* texture2d;
	textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	shaderResourceViewDesc.Format = textureDesc.Format;
	pInitialData.pSysMem = color;
	pInitialData.SysMemPitch = textureWidth * sizeof(XMFLOAT3);

	//���2D�������ݽṹ��,������2D��ȾĿ������
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags =  D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&textureDesc, &pInitialData, &texture2d));

	//������ɫ����Դ��ͼ
	HR(g_pDevice->CreateShaderResourceView(texture2d, nullptr, &textureSRV));

	ReleaseCOM(texture2d);

	return true;
}

ID3D11ShaderResourceView* NoiseTexture::GetSRV()
{
	return textureSRV;
}