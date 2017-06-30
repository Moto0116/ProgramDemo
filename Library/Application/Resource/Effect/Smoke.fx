Texture2D g_Texture : register(t0);
Texture2D g_SkyCLUT : register(t3);
SamplerState g_Sampler : register(s0);

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

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD;
	float4x4 mat    : MATRIX;          // インスタンスごとに設定される行列
	float4 Color    : COLOR;
	uint InstanceId : SV_InstanceID;   // インスタンスＩＤ
};

struct VS_OUTPUT
{
	float4 PosWVP   : SV_POSITION;
	float2 UV		: TEXCOORD;
	float4 Color    : COLOR;
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(In.mat, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;
	Out.Color = In.Color;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float4 FinalColor = 
	g_Texture.Sample(g_Sampler, In.UV);
	FinalColor = FinalColor * g_SkyCLUT.Sample(g_Sampler, float2(g_LightDot.x, 0.0f));
	FinalColor.a *= In.Color.a;
	return FinalColor;

	//float4 FinalColor = g_Texture.Sample(g_Sampler, In.UV);
	//FinalColor.a *= In.Color.a;
	//return FinalColor;
}
