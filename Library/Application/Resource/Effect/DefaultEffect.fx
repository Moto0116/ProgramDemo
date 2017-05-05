#define FAR  1500.0f
#define NEAR 150.0f
#define FOGCOLOR float4(1.0f, 1.0f, 1.0f, 1.0f)

Texture2D g_Texture : register(t0);
Texture2D g_DepthTexture : register(t2);
Texture2D g_SkyCLUT : register(t3);
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

cbuffer Material : register(b3)
{
	float4 g_Diffuse;
	float4 g_Ambient;
	float4 g_Specular;
	float4 g_Emissive;
};

struct VS_INPUT
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 UV     : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 PosWVP   : SV_POSITION;
	float4 Normal   : NORMAL;
	float2 UV       : TEXCOORD;
	float2 LightUV  : TEXCOORD2;
	float ZValue    : TEXCOORD3;
	float Distance  : TEXCOORD4;
	float4 Color    : COLOR;
};



VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.Normal = float4(In.Normal, 1.0f);
	Out.UV = In.UV;

	// ライトビュープロジェクション
	float4x4 LightMat = mul(g_World, g_LightView);
	LightMat = mul(LightMat, g_LightProj);
	float4 LightUVData = mul(float4(In.Pos, 1.0f), LightMat);
	Out.ZValue = LightUVData.z / LightUVData.w;
	Out.LightUV.x = (LightUVData.x / LightUVData.w) * 0.5f + 0.5f;
	Out.LightUV.y = (LightUVData.y / LightUVData.w) * -0.5f + 0.5f;

	// 法線とライトからカラー値を計算
	float3 InvLightDir = normalize(g_LightDir.xyz);
	float3 Normal = normalize(In.Normal.xyz);

	Out.Color = max(g_Ambient, dot(Normal, InvLightDir));

	Out.Distance = clamp((FAR - distance(In.Pos, g_CameraPos.xyz)) / (FAR - NEAR), 0.0f, 1.0f);

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	if (In.ZValue > (g_DepthTexture.Sample(g_Sampler, In.LightUV).r + 0.00006f))
	{
		if (In.LightUV.x >= 0.01 && In.LightUV.x <= 0.99 && In.LightUV.y >= 0.01 && In.LightUV.y <= 0.99)
		{
			In.Color.rgb = In.Color.rgb * 0.7f;
		}
	}

	return g_Texture.Sample(g_Sampler, In.UV) * 
		In.Color * 
		g_SkyCLUT.Sample(g_Sampler, float2(g_LightDot.x, 0.0f)) *
		In.Distance + 
		FOGCOLOR * 
		(1.0f - In.Distance);
}