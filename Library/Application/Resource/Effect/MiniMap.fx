Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer model : register(b0)
{
	matrix g_World;
};

cbuffer MapCamera : register(b4)
{
	matrix g_View;
	matrix g_Proj;
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
	float2 UV       : TEXCOORD;
};



VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);
	Out.Pos = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_Target
{
	return g_Texture.Sample(g_Sampler, In.UV);
}
