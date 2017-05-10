float SpringPower = 0.5f;	// ばねの強さ

Texture2D g_WaveTexture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer model : register(b0)
{
	matrix g_World;
	float4 g_TexelOffset;	// 1テクセル分のオフセット値
	float4 g_AddWavePos;	// 追加する波のテクセル値
	float4 g_AddWaveHeight; // 追加する波の高さ
	matrix g_MapWorld;		// 波マップ 法線マップ描画行列
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

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos  : SV_POSITION;
	float2 UV	: TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Pos = mul(float4(In.Pos, 1.0f), g_MapWorld);
	Out.UV = In.UV;

	Out.Pos.x = (Out.Pos.x / 800) * 2 - 1;
	Out.Pos.y = 1 - (Out.Pos.y / 800) * 2;

	return Out;
}

// 波マップ作成
float4 PS_WAVEMAP(VS_OUTPUT In) : SV_TARGET
{
	// 高さ(R), 速度(G)
	float4 Wave = g_WaveTexture.Sample(g_Sampler, In.UV);

	// 周囲の高さ情報取得
	float H1 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(g_TexelOffset.x,  0.0f			)).r;	// 右
	float H2 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				g_TexelOffset.y )).r;	// 上
	float H3 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(-g_TexelOffset.x, 0.0f			)).r;	// 左
	float H4 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				-g_TexelOffset.y)).r;	// 下

	float WaveSpeed = Wave.y + ((H1 + H2 + H3 + H4) * 0.25f - Wave.x) * SpringPower;
	float WaveHeight = Wave.x + WaveSpeed - 0.1f;

	if (distance(In.UV, g_AddWavePos.xy) < 0.03f)
	{
		WaveSpeed += g_AddWaveHeight.x;
	}

	return float4(WaveHeight, WaveSpeed, 0, 1);
}

// 法線マップ作成
float4 PS_BUMPMAP(VS_OUTPUT In) : SV_TARGET
{
	float H1 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(g_TexelOffset.x,	0.0f			)).r;	// 右
	float H2 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				g_TexelOffset.y )).r;	// 上
	float H3 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(-g_TexelOffset.x, 0.0f			)).r;	// 左
	float H4 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				-g_TexelOffset.y)).r;	// 下

	float tu = 0.5f * (H3 - H1) + 0.5f;
	float tv = 0.5f * (H4 - H2) + 0.5f;

	return float4(
		tu,			// x成分
		1.0f,		// y成分
		tv,			// z成分
		g_WaveTexture.Sample(g_Sampler, In.UV).r * 0.5f + 0.5f);	// 高さ情報
}
