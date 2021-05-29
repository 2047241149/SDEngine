#pragma once

#include <stdio.h>
#include<iostream>
#include<d3d11_1.h>
#include<windows.h>
#include<fstream>
#include<memory.h>

#ifdef SD_BUILD_DLL
	#define SD_API __declspec(dllexport)
#else
	#define SD_API __declspec(dllimport)
#endif