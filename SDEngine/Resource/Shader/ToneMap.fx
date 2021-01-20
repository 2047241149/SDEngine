
//ACES: https://zhuanlan.zhihu.com/p/21983679
//GammaCorrect: https://learnopengl.com/Advanced-Lighting/Gamma-Correction
float3 ACESToneMappingAndGammaCorrect(float3 color, float adaptedLum)
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;

	color *= adaptedLum;
	color = (color * (a * color + b)) / (color * (c * color + d) + e);
	float gammaCorrect = 1.0 / 2.2;
	color = pow(color, gammaCorrect);
	return color;
}