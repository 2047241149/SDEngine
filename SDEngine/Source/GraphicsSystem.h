#pragma once
#ifndef _GRAPHICS_SYSTEM_H
#define _GRAPHICS_SYSTEM_H

class SkyBox;
class RenderTexture;
class Input;
class Quad;
class SSAOManager;
class CascadedShadowsManager;
class SSRGBuffer;
class DebugWindow;
class GeometryBuffer;
class DepthBufferRT;
class Texture;
class GameObject;


#include "DirectxCore.h"
#include"Macro.h"
#include"Light.h"
#include"FPS.h"
#include<memory>
#include "GraphicsConfig.h"
#include "Mesh.h"

class GraphicsSystem
{

private:
	//输入类
	shared_ptr<Input> mInputClass;

	//窗口句柄
	HWND mhwnd;

	shared_ptr<Quad> mQuad;

	//公用纹理资源
	shared_ptr<Texture> whiteTexture;

	//网格数据类
	shared_ptr<GameObject> mOpacitySphereObject;
	shared_ptr<GameObject> mTransSphereObject;
	shared_ptr<GameObject> mHeadObject;
	shared_ptr<GameObject> mSponzaNoBottom;
	shared_ptr<GameObject> mSponzaBottom;
	shared_ptr<GameObject> m_pPointVolume;

	//天空盒子
	shared_ptr<SkyBox> skyBox;

	shared_ptr<RenderTexture> mSSRRT;
	shared_ptr<RenderTexture> mSrcRT;
	shared_ptr<RenderTexture> mLightBuffer;
	shared_ptr<RenderTexture> mGrayShadowMap;
	shared_ptr<CascadedShadowsManager> mCascadeShadowsManager;
	shared_ptr<SSAOManager> ssaoManager;

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
	void RenderSkyBoxPass();
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
