#pragma once

#ifndef _GRAPHICS_CONFIG_H
#define _GRAPHICS_CONFIG_H

//全局变量
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = false;  //是尽可能快渲染还是限制帧渲染
const float SCREEN_FAR = 600.0f;  //视截体远裁面
const float SCREEN_NEAR = 0.1f;  //视截体近裁面
const float CAMERA_SPEED = 35.0f;
const int SHADOW_MAP_SIZE = 1024;
const int CASCADE_SHADOW_MAP_NUM = 3;
const float GAME_ENGINE_UNIT_SCALE = 1.0f;
const float CASCADE_PERCENT[CASCADE_SHADOW_MAP_NUM + 1] = { 0.0f, 0.05f, 0.15f, 0.6f };
static bool bSSAO = true;

//#define POST_EFFECT
//#define SSR
#define DEBUG_GBUFFER 1
#endif // !_GRAPHICS_CONFIG_H

