#pragma once

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

namespace Resource
{
	HRESULT CreateShaderResourceViewFromFile(ID3D11Device* d3dDevice, const wchar_t* szFileName, ID3D11ShaderResourceView** textureView);
}