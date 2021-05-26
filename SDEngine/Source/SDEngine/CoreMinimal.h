#pragma once

#include <stdio.h>
#include<iostream>
#ifdef SD_BUILD_DLL
	#define SD_API __declspec(dllexport)
#else
	#define SD_API __declspec(dllimport)
#endif