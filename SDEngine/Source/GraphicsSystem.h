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
#include "ShadowMap.h"


//ȫ�ֱ���
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = false;  //�Ǿ����ܿ���Ⱦ��������֡��Ⱦ
const float SCREEN_FAR = 400.0f;  //�ӽ���Զ����
const float SCREEN_NEAR = 1.0f;  //�ӽ��������
const float CAMERA_SPEED = 10.0f;
const int LIGHT_MAP_DOWN_SMAPLE = 1;
const int SHADOW_MAP_SIZE = 3072;

//#define POST_EFFECT
//#define SSR
#define DEBUG_GBUFFER

class GraphicsSystem
{

private:
	//������
	shared_ptr<Input> mInputClass;

	//���ھ��
	HWND mhwnd;

	shared_ptr<Quad> mQuad;

	//����������
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
	shared_ptr<ShadowMap> mDirLightShadowMap;

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
