Texture2D g_Texture : register(t0);
Texture2D g_WaterTexture : register(t1);
Texture2D g_SkyTexture : register(t2);
Texture2D g_HeightTexture : register(t3);	// �g�e�N�X�`��(HeightMap�Ƃ��Ďg��)
Texture2D g_BumpTexture : register(t4);		// �@���e�N�X�`��
Texture2D g_ColorTexture : register(t5);	// �J���[�e�N�X�`��
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

cbuffer Water : register(b5)
{
	matrix g_ScalingMat;
	matrix g_TranslateMat;
};

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD;
	float4 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
	float4 Binormal	: BINORMAL;
};

struct VS_OUTPUT
{
	float4 PosWVP		: SV_POSITION;
	float2 UV			: TEXCOORD;
	float3 EyeRay		: TEXCOORD2;
	float3 Eye			: TEXCOORD3;
	float4 ReflectPos	: TEXCOORD4;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;

	Out.EyeRay = In.Pos - g_CameraPos.xyz;

	float4x4 InvMat = mul(g_World, g_ReflectView);
	InvMat = mul(InvMat, g_ReflectProj);
	InvMat = mul(InvMat, g_ScalingMat);
	InvMat = mul(InvMat, g_TranslateMat);
	float4 InvUVData = mul(float4(In.Pos, 1.0f), InvMat);
	Out.ReflectPos = InvUVData;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float3 Normal = normalize(2.0f * g_BumpTexture.Sample(g_Sampler, In.UV).rgb - 1.0f);	// �ϊ����Ď擾

	float2 UV = In.ReflectPos.xy / In.ReflectPos.w;	// ���]�e�N�X�`��UV�̌v�Z

	// ���]�e�N�X�`������F���擾
	float4 FinalColor = g_Texture.Sample(g_Sampler, UV);
	FinalColor.a = 1;

	// ���C�g����
	float3 InvLightDir = normalize(g_LightDir.xyz);
	FinalColor *= max(0.5, dot(Normal, InvLightDir));

	//�X�y�L��������
	float Specula = pow(max(0.0f, dot(Normal, InvLightDir)), 13.0) * 0.8;

	return FinalColor
		* g_WaterTexture.Sample(g_Sampler, In.UV)
		* g_SkyTexture.Sample(g_Sampler, float2(g_LightDot.x, 0.0f))
		+ (Specula * g_WaterTexture.Sample(g_Sampler, In.UV).a);
}