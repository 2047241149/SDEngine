#pragma once
#ifndef _LOG_H
#define _LOG_H
#include<iostream>
#include<d3d11_1.h>
#include<windows.h>
#include<fstream>
using namespace std;


class Log
{
public:

	static void LogShaderCompileInfo(ID3D10Blob* errorMessage, WCHAR* shaderFilename);
};


#endif // _LOG_H
