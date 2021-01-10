#include "TextureSamplerManager.h"


TextureSamplerManager::TextureSamplerManager()
{
	Init();
}

TextureSamplerManager::TextureSamplerManager(const TextureSamplerManager& other)
{

}


TextureSamplerManager::~TextureSamplerManager()
{
	Release();
}

shared_ptr<TextureSamplerManager> TextureSamplerManager::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<TextureSamplerManager>(new TextureSamplerManager());
	}

	return single;
}

bool TextureSamplerManager::Init()
{
	ID3D11SamplerState* samplerState;
	//(1)PointClamp
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(PointClamp, samplerState));


	//(2)BilinearFliterClamp
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(BilinearFliterClamp, samplerState));


	//(3)TrilinearFliterClamp
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(TrilinearFliterClamp, samplerState));


	//(4)PointWrap
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(PointWrap, samplerState));

	//(5)BilinearFliterWrap
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(BilinearFliterWrap, samplerState));

	//(6)TrilinearFliterWrap
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &samplerState));
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(TrilinearFliterWrap, samplerState));


	//(7)
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&samplerDesc, &samplerState);
	mapTextureSampler.insert(pair<TextureSampler, ID3D11SamplerState*>(Anisotropic, samplerState));
	return true;
}


void TextureSamplerManager::Release()
{
	for (auto iterator = mapTextureSampler.begin(); iterator != mapTextureSampler.end(); ++iterator)
	{
		if (nullptr != iterator->second)
		{
			ReleaseCOM(iterator->second);
		}
	}
}


ID3D11SamplerState* TextureSamplerManager::GetTextureSampler(TextureSampler textureSampler)
{
	if (mapTextureSampler.end() == mapTextureSampler.find(textureSampler))
		return nullptr;
	
	return mapTextureSampler[textureSampler];
}

shared_ptr<TextureSamplerManager> TextureSamplerManager::single = nullptr;