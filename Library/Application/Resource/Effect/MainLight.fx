Texture2D g_Texture : register(t0);
Texture2D g_BloomTexture : register(t1);
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
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_LightMatrix, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	return g_Texture.Sample(g_Sampler, In.UV) + g_BloomTexture.Sample(g_Sampler, In.UV);
}