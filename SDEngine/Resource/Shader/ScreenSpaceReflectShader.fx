Texture2D<float4> DiffuseTex:register(t0);
Texture2D<float4> FrontDepthTex:register(t1);
Texture2D<float4> BackDepthTex:register(t2);
Texture2D<float4> ViewPosTex:register(t3);
Texture2D<float4> ViewNormalTex:register(t4);
SamplerState SampleWrapLinear:register(s0);
SamplerState SampleClampPoint:register(s1);

#define MAX_STEPS 500

cbuffer CBMatrix:register(b0)
{
	matrix View;
	matrix Proj;
};


cbuffer CBSSR:register(b1)
{
	float farPlane;
	float nearPlane;
	float2 perspectiveValues;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float2 Tex:TEXCOORD;
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

float DepthBufferConvertToViewDepth(float depth)
{
	float viewDepth = perspectiveValues.x / (depth + perspectiveValues.y);
	return viewDepth;
};


float2 texSize(Texture2D tex)
{
	uint texWidth, texHeight;
	tex.GetDimensions(texWidth, texHeight);
	return float2(texWidth, texHeight);
}

//ת��Ϊ��Ļ�ռ�����
float2 NormalizedDeviceCoordToScreenCoord(float2 ndc, float2 screenSize)
{
	float2 screenCoord;
	screenCoord.x = screenSize.x * (0.5 * ndc.x + 0.5);
	screenCoord.y = screenSize.y * (-0.5 * ndc.y + 0.5);
	return screenCoord;
}

float distanceSquared(float2 a, float2 b)
{
	a -= b;
	return dot(a, a);
}

VertexOut VS(VertexIn ina)
{
	VertexOut outa;

	outa.Pos = float4(ina.Pos.xy,1.0f,1.0f);
	outa.Tex = ina.Tex;
	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	//��ʼ���������ɫ
	float4 reflectColor = float4(0.0, 0.0, 0.0, 0.0f);
	float2 screenSize = texSize(DiffuseTex);
	float2 texcoord = outa.Tex;
	float3 viewPos = ViewPosTex.Sample(SampleClampPoint, outa.Tex).xyz;
	float3 viewNormal= ViewNormalTex.Sample(SampleClampPoint, outa.Tex).xyz;
	float t = 1;
	int2 origin = texcoord * screenSize;
	int2 coord;


	//����������ռ��λ��(�������)�ͷ���
	float3 v0 = viewPos;
	float3 vsNormal = viewNormal;

	//��������صķ���
	float3 eyeToPixel = normalize(v0);

	//���߷���ķ���
	float3 reflRay = normalize(reflect(eyeToPixel, vsNormal));


	//��������յ�
	float3 v1 = v0 + reflRay * farPlane;


	//��Ļ�ռ������
	float4 p0 = mul(float4(v0, 1.0), Proj);
	float4 p1 = mul(float4(v1, 1.0), Proj);
	

	//����ο����դ�� �������� ����ռ�����Ĳ�ֵԭ��(͸�Ӿ���)
	//wΪ����ռ��Zֵ
	float k0 = 1.0 / p0.w;
	float k1 = 1.0 / p1.w;

	p0 *= k0;
	p1 *= k1;

	v0 *= k0;
	v1 *= k1;

	
	p0.xy = NormalizedDeviceCoordToScreenCoord(p0.xy, screenSize.xy);
	p1.xy = NormalizedDeviceCoordToScreenCoord(p1.xy, screenSize.xy);


	//��֤��Ļ�ռ�Ĺ�����ʼ���յ�����һ����λ����
	float ds = distanceSquared(p1.xy, p0.xy);
	p1 += ds < 0.0001 ? 0.01 : 0.0;
	float divisions = length(p1.xy - p0.xy);
	
	float3 dV = (v1 - v0) / divisions;
	float dK = (k1 - k0) / divisions;
	float2 traceDir = (p1 - p0) / divisions;

	float maxSteps = min(divisions, MAX_STEPS);

	while (t < maxSteps)
	{
		coord = origin + traceDir * t;
		if (coord.x > screenSize.x || coord.y > screenSize.y || coord.x < 0 || coord.y < 0)
		{
			break;
		}

		float curDepth = (v0 + dV * t).z;
		float k = k0 + dK * t;
		curDepth /= k;
		texcoord = float2(coord) / screenSize;
		float storeFrontDepth = FrontDepthTex.SampleLevel(SampleClampPoint, texcoord, 0).r;
		storeFrontDepth = DepthBufferConvertToViewDepth(storeFrontDepth);
		float storeBackDepth = BackDepthTex.SampleLevel(SampleClampPoint, texcoord, 0).r;
		storeBackDepth = DepthBufferConvertToViewDepth(storeBackDepth);
		if ((curDepth > storeFrontDepth) && ((curDepth - storeFrontDepth) <= (storeBackDepth - storeFrontDepth)))
		{
			reflectColor = DiffuseTex.SampleLevel(SampleClampPoint, texcoord, 0);	
			reflectColor.a = 0.4;
			break;	
		}
		t++;
	}

	return reflectColor;
}