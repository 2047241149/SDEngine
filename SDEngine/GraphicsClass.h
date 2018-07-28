#pragma once
#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

#define POST_EFFECT
//#define SSR
//#define DEBUG_GBUFFER

#include"D3DClass.h"
#include"GameObject.h"
#include"Macro.h"
#include"Light.h"
#include"FPS.h"
#include"GraphicsClass.h"
#include"Input.h"
#include"ShaderManager.h"
#include"SDKmesh.h"
#include<DXUT.h>
#include<DXUTgui.h>
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

//全局变量
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = false;  //是尽可能快渲染还是限制帧渲染
const float SCREEN_FAR = 800.0f;  //视截体远裁面
const float SCREEN_NEAR = 1.0f;  //视截体近裁面
const float CAMERA_SPEED = 10.0f;

class GraphicsClass
{

private:
	//输入类
	shared_ptr<Input> mInputClass;

	//窗口句柄
	HWND mhwnd;

	shared_ptr<Quad> mQuad;

	//网格数据类
	shared_ptr<GameObject> mSphereObject;
	shared_ptr<GameObject> mHeadObject;
	shared_ptr<GameObject> mSponzaNoBottom;
	shared_ptr<GameObject> mSponzaBottom;
	shared_ptr<RenderTexture> mSSRRT;
	shared_ptr<RenderTexture> mSrcRT;

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
	bool Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance);


private:
	void RenderFBXMesh();
	void RenderOpacity();
	void RenderTransparency();
	void RenderGeneralTransparency();
	void RenderGeometryPass();
	void RenderLightingPass();
	void RenderPostEffectPass();
	void RenderDebugWindow();
	void RenderSSRPass();
	void RenderSSRBufferPass();
	void RenderSceneBackDepthBuffer();

	void RenderSSR();
	void InitDebugConsole();
	void CloseDebugConsole();

public:
	GraphicsClass(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

public:
	void Render();
	bool Frame(); 

};
#endif // !_GRAPHICS_CLASS_H
