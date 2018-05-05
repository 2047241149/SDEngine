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
	//观察角度阀值(一定角度下观察不到反射现象)
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
	//相机空间的位置和法线
	float3 vsPos = outa.viewPos.xyz;
	float3 vsNormal = outa.viewNormal.xyz;

	//计算相机到像素的方向
	float3 eyeToPixel = normalize(vsPos);

	//计算反射的方向
	float3 vsReflect = reflect(eyeToPixel, vsNormal);

	//初始化反射的颜色
	float4 reflectColor = float4(0.0, 0.0, 0.0, 1.0f);


	//大于一定的角度才产生SSR反射
	if (vsReflect.z >= viewAngleThresshold)
	{
		//计算反射影像的混合值
		float viewAngleThresholdInv = 1.0 - viewAngleThresshold;
		float viewAngleFade = saturate(3.0 * (vsReflect.z - viewAngleThresshold) / viewAngleThresholdInv);

		//将反射点从相机空间变换到NDC空间,以求出ndc空间的反射向量
		float3 vsPosReflect = vsPos + vsReflect;
		float3 ndcPosReflect = mul(float4(vsPosReflect, 1.0), Proj).xyz / vsPosReflect.z;
		float3 ndcReflect = ndcPosReflect - outa.ndcPos;


		//将ndcReflect (xy)变为一个合适的大小
		float pixelSize = 2.0 / 600.0;
		float ndcReflectScale = pixelSize / length(ndcReflect.xy);
		ndcReflect = ndcReflect * ndcReflectScale;

		//计算在屏幕空间(ScrrenSpace)的第一个(反射方向)采样点
		float2 ssSamPos = (outa.ndcPos + ndcReflect).xy;
		ssSamPos = ssSamPos * float2(0.5, -0.5) + float2(0.5,0.5);

		//在屏幕空间反射光线每一步移动的距离(更确切的说是两步，对应于 2.0 / texRes.y)
		float2 ssStep = ndcReflect.xy * float2(0.5, -0.5);

		float4 rayPlane;
		float3 vRight = cross(eyeToPixel, vsReflect);
		rayPlane.xyz = normalize(cross(vsReflect, vRight));
		rayPlane.w = dot(rayPlane.xyz, vsPos);
		

		//在屏幕空间进行位移，并将在屏幕空间得到的数据还原到相机空间而进行光线求交
		for (int nStep = 0; nStep < 1024; nStep++)
		{
			//目前的深度
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