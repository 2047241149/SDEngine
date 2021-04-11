
	

static const int BLUR_RADIUS = 2;
static const int BLUR_SIZE = BLUR_RADIUS * 2 + 1;


Texture2D srcTexture:register(t0);
RWTexture2D<float4> destTexture : register(u0);

#define N 256
#define CacheSize (N + 2 * BLUR_RADIUS)
groupshared float4 gCache[CacheSize];

[numthreads(N,1,1)]
void CS_HorizontalBlur(int3 dispatchThreadID : SV_DispatchThreadID,int3 groupThreadID: SV_GroupThreadID)
{
	const float BlurWeight[BLUR_SIZE] =
	{
		0.0545f, 0.2442f, 0.4026f, 0.2442f,	0.0545f
	};
	//图像的左边缘的像素填入GSM
	if (groupThreadID.x < BLUR_RADIUS)
	{
		int x = max(dispatchThreadID.x - BLUR_RADIUS, 0);
		gCache[groupThreadID.x] = srcTexture[int2(x, dispatchThreadID.y)];
	}	
	//图像右边缘的像素填入GSM
	if (groupThreadID.x >= N - BLUR_RADIUS)
	{
		int x = min(dispatchThreadID.x + BLUR_RADIUS, srcTexture.Length.x - 1);
		gCache[groupThreadID.x + 2 * BLUR_RADIUS] = srcTexture[int2(x, dispatchThreadID.y)];
	}

	//图像中间的像素填入GSM
	gCache[groupThreadID.x + BLUR_RADIUS] = srcTexture[min(dispatchThreadID.xy, srcTexture.Length.xy - 1)];

	//等待同一个group的所有渲染线程执行完
	GroupMemoryBarrierWithGroupSync();

	//模糊
	float4 blurColor = float4(0, 0, 0, 0);
	
	[unroll]
	for (int i = - BLUR_RADIUS; i <= BLUR_RADIUS; ++i)
	{
		int index1 = groupThreadID.x + i + BLUR_RADIUS;
		int index2 = BLUR_RADIUS + i;
		blurColor += gCache[index1] * BlurWeight[index2];
	}

	blurColor.a = 1.0;
	destTexture[dispatchThreadID.xy] = blurColor;
}



[numthreads(1, N, 1)]
void CS_VerticalBlur(int3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	const float BlurWeight[BLUR_SIZE] =
	{
		0.0545f, 0.2442f, 0.4026f, 0.2442f,	0.0545f
	};
	//图像的上边缘的像素填入GSM
	if (groupThreadID.y < BLUR_RADIUS)
	{
		int y = max(dispatchThreadID.y - BLUR_RADIUS, 0);
		gCache[groupThreadID.y] = srcTexture[int2(dispatchThreadID.x, y)];
	}

	//图像右边缘的像素填入GSM
	if (groupThreadID.y >= N - BLUR_RADIUS)
	{
		int y = min(dispatchThreadID.y + BLUR_RADIUS, srcTexture.Length.y - 1);
		gCache[groupThreadID.y + 2 * BLUR_RADIUS] = srcTexture[int2(dispatchThreadID.x, y)];
	}

	//图像中间的像素填入GSM
	gCache[groupThreadID.y + BLUR_RADIUS] = srcTexture[min(dispatchThreadID.xy, srcTexture.Length.xy - 1)];

	//等待同一个group的所有渲染线程执行完
	GroupMemoryBarrierWithGroupSync();

	//模糊
	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -BLUR_RADIUS; i <= BLUR_RADIUS; ++i)
	{
		int index1 = groupThreadID.y + i + BLUR_RADIUS;
		int index2 = BLUR_RADIUS + i;
		blurColor += gCache[index1] * BlurWeight[index2];
	}

	blurColor.a = 1.0;
	destTexture[dispatchThreadID.xy] = blurColor;
}