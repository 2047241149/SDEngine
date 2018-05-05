#pragma once
#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

//#define POST_EFFECT

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
#include<memory>
#include"Quad.h"
#include"Debugwindow.h"
#include"ColorBufferRT.h"
#include "Camera.h"

//ȫ�ֱ���
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = false;  //�Ǿ����ܿ���Ⱦ��������֡��Ⱦ
const float SCREEN_FAR = 300.0f;  //�ӽ���Զ����
const float SCREEN_NEAR = 1.0f;  //�ӽ��������
const float CAMERA_SPEED = 3.0f;

class GraphicsClass
{

private:
	//������
	shared_ptr<Input> mInputClass;

	//���ھ��
	HWND mhwnd;

	shared_ptr<Quad> mQuad;

	//����������
	shared_ptr<GameObject> mSphereObject;

	shared_ptr<GameObject> mHeadObject;

	shared_ptr<GameObject> mSponzaNoBottom;
	shared_ptr<GameObject> mSponzaBottom;

	shared_ptr<ColorBufferRT> mDownSampleRT;
	shared_ptr<ColorBufferRT> mUpSampleRT;
	shared_ptr<ColorBufferRT> mSrcRT;
	shared_ptr<ColorBufferRT> mFirstBlurRT;
	shared_ptr<ColorBufferRT> mSceondBlurRT;

	shared_ptr<ColorBufferRT> mSSRRT;

	//GeometryBuffer
	shared_ptr<GeometryBuffer> mGeometryBuffer;
	
	MaterialType materialType = MaterialType::DIFFUSE;
	MaterialType preMaterialType = MaterialType::DIFFUSE;

	shared_ptr<DebugWindow> mDebugWindow;

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
public:
	GraphicsClass(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

public:
	void Render();
	bool Frame(); 

};
#endif // !_GRAPHICS_CLASS_H
