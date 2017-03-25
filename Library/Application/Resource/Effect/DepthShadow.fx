cbuffer model : register(b0)
{
	matrix g_World;
};

cbuffer camera : register(b1)
{
    matrix g_View;
    matrix g_Proj;
};

cbuffer light : register(b2)
{
	float4 g_LightPos;
	float4 g_LightDir;
	matrix g_LightView;
	matrix g_LightProj;
};

struct VS_INPUT
{
	float3 Pos   : POSITION;
	float3 Normal : NORMAL;   
	float2 UV : TEXCOORD;   
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT In)
{
    VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_LightView);
	Mat = mul(Mat, g_LightProj);
	Out.Pos = mul(float4(In.Pos, 1.0f), Mat);

	return Out;
}

float PS(VS_OUTPUT In) : SV_TARGET
{
	return In.Pos.z / In.Pos.w;
}
