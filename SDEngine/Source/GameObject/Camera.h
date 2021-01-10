
#pragma once
#ifndef  _CAMERA_H
#define _CAMERA_H
//������װ�������λ�ú���ת��
#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;
class Camera
{
public:
	//���������ռ��λ�úͷ�������
	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;

	//Proj����
	float mFovY;
	float mAspect;
	float mNearPlane;
	float mFarPlane;

	float mScreenWidth, mScreenHeight;


	//��������任����
	XMMATRIX mViewMatrix;
	XMMATRIX mUIViewMatrix;
	XMMATRIX mProjMatrix;  //ͶӰ����

public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	static shared_ptr<Camera> Get();
public:
	static shared_ptr<Camera> m_pCamera;
public:
	//���λ��
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);
	void SetProjParams(float fovY = XM_PIDIV4, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 500.0f);
	void SetUIOrthoParams(float screenWidth, float screenHeight);
	XMFLOAT3 GetPosition()const;
	XMVECTOR GetPositionXM()const;

	//��ȡ����Ļ�������(Up,Look,Right)
	XMFLOAT3 GetUp()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetLook()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetRightXM()const;

	//��ȡ���������
	float GetNearPlane()const;
	float GetFarPlane()const;

	//ͨ�����������ռ��λ�ã�Ŀ��㣬�Լ�����������������任����
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	
	//��ȡ����任����
	XMMATRIX GetViewMatrix()const;

	//��ȡUI����任����
	XMMATRIX GetUIViewMatrix()const;
	
	//��ȡUI����ͶӰ����
	XMMATRIX GetUIOrthoMatrix()const;

	//��ȡ͸��ͶӰ����
	XMMATRIX GetProjectionMatrix()const;

	//��������任����
	void UpdateViewMatrix();

	//�����ƶ�(�����������ƶ�)
	void Strafe(float d);

	//ǰ���ƶ�(����LookAt�����ƶ�)
	void Walk(float d);

	//�����ƶ�(��������ռ��Y���ƶ�)
	void UpDown(float d);

	//���������������ת
	void Pitch(float angle);

	//������ռ��Y�������ת,ע����ת�Ƕ�Ӧ���ڳ�ʼ���������90��֮��,������������������(-90.0f<=RotateAngle<=90.0f)
	void RotateY(float angle);
};
#endif 

#define	GCamera (Camera::Get())
