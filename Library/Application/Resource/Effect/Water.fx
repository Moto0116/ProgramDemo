TextureCube g_Texture : register(t0);
Texture2D g_WaterTexture : register(t1);
Texture2D g_SkyTexture : register(t2);
SamplerState g_Sampler : register(s0);

cbuffer model : register(b0)
{
	matrix g_World;
};

cbuffer camera : register(b1)
{
	matrix g_View;
	matrix g_Proj;
	float4 g_CameraPos;
	float4 g_CameraDir;
	float4 g_Aspect;
};

cbuffer light : register(b2)
{
	float4 g_LightPos;
	float4 g_LightDir;
	matrix g_LightView;
	matrix g_LightProj;
	matrix g_LightMatrix;
	float4 g_LightDot;
};

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 PosWVP   : SV_POSITION;
	float2 UV		: TEXCOORD;
	float3 EyeRay	: TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;

	Out.EyeRay = mul(g_World, float4(In.Pos, 1.0f)).xyz - g_CameraPos.xyz;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float3 vReflect = reflect(normalize(In.EyeRay), normalize(float3(0, 1, 0)));
	float4 FinalColor = g_Texture.Sample(g_Sampler, vReflect);
	FinalColor.a = 0.5f;	// è≠ÇµîñÇ≠Ç∑ÇÈ

	return FinalColor 
		* g_WaterTexture.Sample(g_Sampler, In.UV)
		* g_SkyTexture.Sample(g_Sampler, float2(g_LightDot.x, 0.0f));
}