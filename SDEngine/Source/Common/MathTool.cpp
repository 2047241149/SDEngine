#include"MathTool.h"



//返回一个矩阵的逆矩阵的转置
XMMATRIX FMath::GetInvenseTranspose(CXMMATRIX matrix)
{
	XMMATRIX A = matrix;

	//将矩阵A的第四行置零,因为向量是无法进行平移的
	A.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX MaInvense = XMMatrixInverse(&det, A);
	return XMMatrixTranspose(MaInvense);
}

//返回一个矩阵的逆矩阵
XMMATRIX FMath::GetInvense(CXMMATRIX matrix)
{
	XMMATRIX A = matrix;

	//将矩阵A的第四行置零,因为向量是无法进行平移的
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

