Texture2D DiffuseTex:register(t0);  
Texture2D DepthTex:register(t1);
SamplerState SampleWrapLinear:register(s0);
SamplerState SampleClampPoint:register(s1);


cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
	float3 cameraPos;
	float pad1;
	float4 dirLightColor;
	float3 dirLightDir;
	float pad2;
	float3 ambientLight;
	float pad3;
};


cbuffer CBSSR:register(b1)
{
	//�۲�Ƕȷ�ֵ(һ���Ƕ��¹۲첻����������)
	float viewAngleThresshold;
	float edgeDistThresshold;
	float depthBias;
	float reflectScale;
	float4 perspectiveValues;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float3 Color:COLOR;
	float3 Normal:NORMAL;
	float3 Tangent:TANGENT;
	float2 Tex:TEXCOORD;
};


struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 viewPos:TEXCOORD0;
	float3 viewNormal:TEXCOORD1;
	float3 ndcPos:TEXCOORD2;
};

float DepthBufferConvertToViewDepth(float depth)
{
	float viewDepth = perspectiveValues.z / (depth + perspectiveValues.w);
	return viewDepth;
};


static const int nNumSteps = 800;

float3 CalcViewPos(float2 csPos, float viewDepth)
{
	float3 position;
	position.xy = csPos.xy * perspectiveValues.xy * viewDepth;
	position.z = viewDepth;
	return position;
};

float2 texSize(Texture2D tex)
{
	uint texWidth, texHeight;
	tex.GetDimensions(texWidth, texHeight);
	return float2(texWidth, texHeight);
}


VertexOut VS(VertexIn ina)
{
	VertexOut outa;

	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.Pos = mul(outa.Pos, View);

	outa.viewPos = outa.Pos.xyz;

	outa.Pos = mul(outa.Pos, Proj);

	outa.viewNormal = mul(ina.Normal, (float3x3)World);
	outa.viewNormal = mul(outa.viewNormal, (float3x3)View);

	outa.ndcPos = outa.Pos.xyz / outa.Pos.w;

	return outa;
}


float4 PS(VertexOut outa) : SV_Target
{
	//����ռ��λ�úͷ���
	float3 vsPos = outa.viewPos.xyz;
	float3 vsNormal = outa.viewNormal.xyz;

	//������������صķ���
	float3 eyeToPixel = normalize(vsPos);

	//���㷴��ķ���
	float3 vsReflect = reflect(eyeToPixel, vsNormal);

	//��ʼ���������ɫ
	float4 reflectColor = float4(0.0, 0.0, 0.0, 1.0f);


	//����һ���ĽǶȲŲ���SSR����
	if (vsReflect.z >= viewAngleThresshold)
	{
		//���㷴��Ӱ��Ļ��ֵ
		float viewAngleThresholdInv = 1.0 - viewAngleThresshold;
		float viewAngleFade = saturate(3.0 * (vsReflect.z - viewAngleThresshold) / viewAngleThresholdInv);

		//������������ռ�任��NDC�ռ�,�����ndc�ռ�ķ�������
		float3 vsPosReflect = vsPos + vsReflect;
		float3 ndcPosReflect = mul(float4(vsPosReflect, 1.0), Proj).xyz / vsPosReflect.z;
		float3 ndcReflect = ndcPosReflect - outa.ndcPos;


		//��ndcReflect (xy)��Ϊһ�����ʵĴ�С
		float pixelSize = 2.0 / 600.0;
		float ndcReflectScale = pixelSize / length(ndcReflect.xy);
		ndcReflect = ndcReflect * ndcReflectScale;

		//��������Ļ�ռ�(ScrrenSpace)�ĵ�һ��(���䷽��)������
		float2 ssSamPos = (outa.ndcPos + ndcReflect).xy;
		ssSamPos = ssSamPos * float2(0.5, -0.5) + float2(0.5,0.5);

		//����Ļ�ռ䷴�����ÿһ���ƶ��ľ���(��ȷ�е�˵����������Ӧ�� 2.0 / texRes.y)
		float2 ssStep = ndcReflect.xy * float2(0.5, -0.5);

		float4 rayPlane;
		float3 vRight = cross(eyeToPixel, vsReflect);
		rayPlane.xyz = normalize(cross(vsReflect, vRight));
		rayPlane.w = dot(rayPlane.xyz, vsPos);
		

		//����Ļ�ռ����λ�ƣ���������Ļ�ռ�õ������ݻ�ԭ������ռ�����й�����
		for (int nStep = 0; nStep < 1024; nStep++)
		{
			//Ŀǰ�����
			float curDepth = DepthTex.SampleLevel(SampleClampPoint, ssSamPos, 0).x;

			float curViewDepth = DepthBufferConvertToViewDepth(curDepth);
			float2 curNdcPos = outa.ndcPos.xy + ndcReflect.xy * ((float)nStep + 1.0);
			float3 curViewPos = CalcViewPos(curNdcPos, curViewDepth);


			if (rayPlane.w >= dot(rayPlane.xyz, curViewPos) + depthBias)
			{
				float3 vsFinalPos = vsPos + (vsReflect / abs(vsReflect.z)) * abs(curViewDepth - vsPos.z + depthBias);
				float2 ndcFinalPos = vsFinalPos.xy / perspectiveValues.xy / vsFinalPos.z;
				ssSamPos = ndcFinalPos.xy * float2(0.5, -0.5) + float2(0.5, 0.5);

				reflectColor.xyz = DiffuseTex.SampleLevel(SampleWrapLinear, ssSamPos, 0).xyz;

				break;
			}

			ssSamPos = ssSamPos + ssStep;
		}
	}

	reflectColor.a = 0.5;

	return reflectColor;
}