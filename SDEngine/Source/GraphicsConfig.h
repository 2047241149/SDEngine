#pragma once

#ifndef _GRAPHICS_CONFIG_H
#define _GRAPHICS_CONFIG_H

//ȫ�ֱ���
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = false;  //�Ǿ����ܿ���Ⱦ��������֡��Ⱦ
const float SCREEN_FAR = 200.0f;  //�ӽ���Զ����
const float SCREEN_NEAR = 1.0f;  //�ӽ��������
const float CAMERA_SPEED = 10.0f;
const int SHADOW_MAP_SIZE = 1024;
const int CASCADE_SHADOW_MAP_NUM = 3;
const float CASCADE_PERCENT[CASCADE_SHADOW_MAP_NUM + 1] = { 0.0f, 0.15f, 0.3f, 1.0f };

//#define POST_EFFECT
//#define SSR
#define DEBUG_GBUFFER

#endif // !_GRAPHICS_CONFIG_H

