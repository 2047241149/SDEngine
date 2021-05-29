#pragma once
#ifndef _LOG_H
#define _LOG_H

#include "CoreMinimal.h"
#include "spdlog/logger.h"

using namespace std;
using namespace spdlog;

class SD_API Log
{
public:
	Log();
	~Log();

public:
	static void LogShaderCompileInfo(ID3D10Blob* errorMessage, WCHAR* shaderFilename);

public:
	static shared_ptr<Log> Get();

	template<typename... Types>
	static void Error(const Types&... args)
	{
		Get()->coreLogger->error(args...);
	}

	template<typename... Types>
	static void Warn(const Types&... args)
	{
		Get()->coreLogger->warn(args...);
	}

	template<typename... Types>
	static void Info(const Types&... args)
	{
		Get()->coreLogger->info(args...);
	}

	template<typename... Types>
	static void Trace(const Types&... args)
	{
		Get()->coreLogger->trace(args...);
	}

private:
	bool Init();

private:
	static shared_ptr<Log> single;
	shared_ptr<logger> coreLogger;
};

#endif // _LOG_H
