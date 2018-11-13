#pragma once
#ifndef _GRAPHICS_SYSTEM_H
#define _GRAPHICS_SYSTEM_H

#include "DirectxCore.h"
#include "GameObjectManager.h"
#include"Macro.h"
#include"Light.h"
#include"FPS.h"
#include"Input.h"
#include"ShaderManager.h"
#include"GeometryBuffer.h"
#include "DepthBufferRT.h"
#include<memory>
#include"Quad.h"
#include"Debugwindow.h"
#include "RenderTexture.h"
#include "Camera.h"
#include "SSRGBuffer.h"
#include "DirectionWave.h"
#include "CircleWave.h"
#include "GerstnerWave.h"
#include "GerstnerWaveCS.h"
#include "GrussianBlurCS.h"
#include "RWRenderTexture.h"
#include "LightManager.h"
#include "CascadeShadowMapManager.h"
#include "GraphicsConfig.h"
#include "NoiseTexture.h"

const int SSAO_NOISE_TEXTURE_SIZE = 16;
const int SSAO_VEC_SCALE_NUM = 64;
class GraphicsSystem
{

private:
	//输入类
	shared_ptr<Input> mInputClass;

	//窗口句柄
	HWND mhwnd;

	shared_ptr<Quad> mQuad;

	//网格数据类
	shared_ptr<GameObject> mOpacitySphereObject;
	shared_ptr<GameObject> mTransSphereObject;
	shared_ptr<GameObject> mHeadObject;
	shared_ptr<GameObject> mSponzaNoBottom;
	shared_ptr<GameObject> mSponzaBottom;
	shared_ptr<GameObject> m_pPointVolume;

	shared_ptr<RenderTexture> mSSRRT;
	shared_ptr<RenderTexture> mSrcRT;
	shared_ptr<RenderTexture> mLightBuffer;
	shared_ptr<RenderTexture> mGrayShadowMap;
	shared_ptr<RenderTexture> ssaoRT;
	shared_ptr<NoiseTexture> ssaoNoiseTexture;
	XMFLOAT3 ssaoSampleArray[SSAO_VEC_SCALE_NUM];
	shared_ptr<CascadedShadowsManager> mCascadeShadowsManager;

	//GeometryBuffer
	shared_ptr<GeometryBuffer> mGeometryBuffer;

	//SSRGBuffer
	shared_ptr<SSRGBuffer> mSSRBuffer;
	
	MaterialType materialType = MaterialType::DIFFUSE;
	MaterialType preMaterialType = MaterialType::DIFFUSE;

	shared_ptr<DebugWindow> mDebugWindow;

	shared_ptr<DepthBufferRT>  mBackDepthBufferRT;

	int m_nScreenWidth, m_nScreenHeight;


private:
	bool Init(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance);


private:
	void RenderOpacity();
	void RenderTransparency();
	void RenderGeneralTransparency();
	void RenderGeometryPass();
	void RenderLightingPass();
	void RenderFinalShadingPass();
	void RenderPostEffectPass();
	void RenderDebugWindow();
	void RenderSSRPass();
	void RenderSSAOPass();
	void RenderSSRBufferPass();
	void RenderSceneBackDepthBuffer();
	void RenderPointLightPass();
	void RenderDirLightPass();
	void RenderShadowMapPass();
	void RenderSSR();
	void InitDebugConsole();
	void CloseDebugConsole();

public:
	GraphicsSystem(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance);
	GraphicsSystem(const GraphicsSystem&);
	~GraphicsSystem();

public:
	void Render();
	bool Frame(); 

};
#endif // !_GRAPHICS_CLASS_H
