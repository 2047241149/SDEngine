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

	// 获取指向错误信息文本的指针
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 获取错误信息文本的长度
	bufferSize = errorMessage->GetBufferSize();

	// 创建一个txt,用于写入错误信息
	fout.open("shader-error.txt");

	//想txt文件写入错误信息
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 关闭文件
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	//弹出提醒的小窗口
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

shared_ptr<Log> Log::single = nullptr;

