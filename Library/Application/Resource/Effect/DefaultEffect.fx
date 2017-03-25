Texture2D g_Texture : register(t0);
Texture2D g_DepthTexture : register(t2);
SamplerState g_Sampler : register(s0);

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

cbuffer Material : register(b3)
{
	float4 g_Diffuse;
	float4 g_Ambient;
	float4 g_Specular;
	float4 g_Emissive;
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
	float4 Normal : NORMAL;
	float2 UV : TEXCOORD;
	float4 LightUV : TEXCOORD2;
	float4 Color : COLOR;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Pos = mul(float4(In.Pos, 1.0f), g_World);
	Out.Pos = mul(Out.Pos, g_View);
	Out.Pos = mul(Out.Pos, g_Proj);
	Out.Normal = float4(In.Normal, 1.0f);
	Out.UV = In.UV;

	// ライトビュープロジェクション
	float4x4 Mat = mul(g_World, g_LightView);
	Mat = mul(Mat, g_LightProj);
	Out.LightUV = mul(float4(In.Pos, 1.0f), Mat);

	// 法線とライトからカラー値を計算
	float3 InvLightDir = -normalize(g_LightDir.xyz);
	float3 Normal = normalize(In.Normal.xyz);
	Out.Color = max(g_Ambient, dot(Normal, InvLightDir));

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float4 ShadowColor = In.Color;
	float ZValue = In.LightUV.z / In.LightUV.w;

	float2 TransTexCoord;
	TransTexCoord.x = (In.LightUV.x / In.LightUV.w + 1.0f) * 0.5f;
	TransTexCoord.y = (1.0f - In.LightUV.y / In.LightUV.w) * 0.5f;
	float SM_Z = g_DepthTexture.Sample(g_Sampler, TransTexCoord).r;

	if (ZValue > SM_Z + 0.005f)
	{
		ShadowColor.rgb = In.Color.rgb * 0.5f;
	}

	return g_Texture.Sample(g_Sampler, In.UV) * ShadowColor;
}