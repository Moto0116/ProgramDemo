float SpringPower = 0.5f;	// �΂˂̋���

Texture2D g_WaveTexture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer model : register(b0)
{
	matrix g_World;
	float4 g_TexelOffset;	// 1�e�N�Z�����̃I�t�Z�b�g�l
	float4 g_AddWavePos;	// �ǉ�����g�̃e�N�Z���l
	float4 g_AddWaveHeight; // �ǉ�����g�̍���
	matrix g_MapWorld;		// �g�}�b�v �@���}�b�v�`��s��
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

// �g�}�b�v�쐬
float4 PS_WAVEMAP(VS_OUTPUT In) : SV_TARGET
{
	// ����(R), ���x(G)
	float4 Wave = g_WaveTexture.Sample(g_Sampler, In.UV);

	// ���͂̍������擾
	float H1 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(g_TexelOffset.x,  0.0f			)).r;	// �E
	float H2 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				g_TexelOffset.y )).r;	// ��
	float H3 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(-g_TexelOffset.x, 0.0f			)).r;	// ��
	float H4 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				-g_TexelOffset.y)).r;	// ��

	float WaveSpeed = Wave.y + ((H1 + H2 + H3 + H4) * 0.25f - Wave.x) * SpringPower;
	float WaveHeight = Wave.x + WaveSpeed - 0.1f;

	if (distance(In.UV, g_AddWavePos.xy) < 0.03f)
	{
		WaveSpeed += g_AddWaveHeight.x;
	}

	return float4(WaveHeight, WaveSpeed, 0, 1);
}

// �@���}�b�v�쐬
float4 PS_BUMPMAP(VS_OUTPUT In) : SV_TARGET
{
	float H1 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(g_TexelOffset.x,	0.0f			)).r;	// �E
	float H2 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				g_TexelOffset.y )).r;	// ��
	float H3 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(-g_TexelOffset.x, 0.0f			)).r;	// ��
	float H4 = g_WaveTexture.Sample(g_Sampler, In.UV + float2(0.0f,				-g_TexelOffset.y)).r;	// ��

	float tu = 0.5f * (H3 - H1) + 0.5f;
	float tv = 0.5f * (H4 - H2) + 0.5f;

	return float4(
		tu,			// x����
		1.0f,		// y����
		tv,			// z����
		g_WaveTexture.Sample(g_Sampler, In.UV).r * 0.5f + 0.5f);	// �������
}
