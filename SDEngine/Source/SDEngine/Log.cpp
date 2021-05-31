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

	// ��ȡָ�������Ϣ�ı���ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// ��ȡ������Ϣ�ı��ĳ���
	bufferSize = errorMessage->GetBufferSize();

	// ����һ��txt,����д�������Ϣ
	fout.open("shader-error.txt");

	//��txt�ļ�д�������Ϣ
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// �ر��ļ�
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	//�������ѵ�С����
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

shared_ptr<Log> Log::single = nullptr;