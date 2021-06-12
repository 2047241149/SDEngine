#pragma once


#include<DirectXMath.h>
#include<d3dcompiler.h>
#include<stdio.h>
#include<iostream>
#include<d3d11.h>
#include<d3d11_1.h>
#include<d3d11_2.h>
#include<windows.h>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<memory.h>
#include<map>
#include<functional>
#include<Windows.h>
#include<windowsx.h>
#include<ctime>

#include "SDEngine/Common/DirectxCore.h"
#include "SDEngine/Common/Macro.h"
#include "SDEngine/Common/MathTool.h"
#include "SDEngine/Common/CommomVertexFormat.h"
#include "SDEngine/Common/CommonFunction.h"
using namespace std;
using namespace DirectX;

#ifdef SD_BUILD_DLL
	#define SD_API
#else
	#define SD_API
#endif