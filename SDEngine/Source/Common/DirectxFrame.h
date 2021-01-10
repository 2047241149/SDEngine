#pragma once
#ifndef _DIRECTX_FRAME_H
#define _DIRECTX_FRAME_H
#include<d3d11_2.h>


namespace DirectXFrame
{
	HRESULT CreateRasterizerState(ID3D11Device* pDevice, ID3D11RasterizerState** ppRasterizerState, D3D11_CULL_MODE = D3D11_CULL_BACK,
		D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID, bool bDepthClipEnable = true);
}


#endif // !_DIRECTX_FRAME_H
