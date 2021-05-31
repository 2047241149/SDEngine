#include"MathTool.h"


//����һ�������������ת��
XMMATRIX FMath::GetInvenseTranspose(CXMMATRIX matrix)
{
	XMMATRIX A = matrix;

	//������A�ĵ���������,��Ϊ�������޷�����ƽ�Ƶ�
	A.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX MaInvense = XMMatrixInverse(&det, A);
	return XMMatrixTranspose(MaInvense);
}

//����һ������������
XMMATRIX FMath::GetInvense(CXMMATRIX matrix)
{
	XMMATRIX A = matrix;

	//������A�ĵ���������,��Ϊ�������޷�����ƽ�Ƶ�
	A.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX MaInvense = XMMatrixInverse(&det, A);
	return MaInvense;
}


float FMath::Max(float left, float right)
{
	return left >= right ? left : right;
}

float FMath::lerp(float left, float right, float percent)
{
	return right * percent + (1 - percent) * left;
}

float FMath::Log2(float num)
{
	return log2(num);
}