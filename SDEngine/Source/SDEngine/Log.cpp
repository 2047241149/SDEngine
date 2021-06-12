#include "Log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Log::Log()
{
	Init();
}

Log::~Log()
{

}

shared_ptr<Log> Log::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<Log>(new Log());
	}

	return single;
}

bool Log::Init()
{
	set_pattern("%^[%T] %n: %v%$");
	set_level(level::level_enum::trace);
	coreLogger = spdlog::stderr_color_mt("coreLogger");
	return true;
}

void Log::LogShaderCompileInfo(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = (unsigned long)errorMessage->GetBufferSize();
	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

shared_ptr<Log> Log::single = nullptr;