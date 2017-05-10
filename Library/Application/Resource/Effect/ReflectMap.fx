Texture2D g_Texture : register(t0);
Texture2D g_WaterTexture : register(t1);
Texture2D g_SkyTexture : register(t2);
Texture2D g_HeightTexture : register(t3);	// 波テクスチャ(HeightMapとして使う)
Texture2D g_BumpTexture : register(t4);		// 法線テクスチャ
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
	matrix g_ReflectView;
	matrix g_ReflectProj;
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

cbuffer Water : register(b5)
{
	matrix g_ScalingMat;
	matrix g_TranslateMat;
};

struct VS_INPUT
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 UV     : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD; 
	float4 WorldPos : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_ReflectView);
	Mat = mul(Mat, g_ReflectProj);
	Out.Pos = mul(float4(In.Pos, 1.0f), Mat);
	Out.Normal = In.Normal;
	Out.UV = In.UV;

	Out.WorldPos = mul(float4(In.Pos, 1.0f), g_World);

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	return g_Texture.Sample(g_Sampler, In.UV);
}