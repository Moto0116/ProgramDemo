TextureCube g_Texture : register(t0);
Texture2D g_WaterTexture : register(t1);
Texture2D g_SkyTexture : register(t2);
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
	float4 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
	float4 Binormal	: BINORMAL;
};

struct VS_OUTPUT
{
	float4 PosWVP   : SV_POSITION;
	float2 UV		: TEXCOORD;
	float3 EyeRay	: TEXCOORD2;
	float3 Eye      : TEXCOORD3;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;

	// �J��������I�u�W�F�N�g�ւ̌���
	Out.EyeRay = In.Pos - g_CameraPos.xyz;

	// �I�u�W�F�N�g����J�����ւ̌���
	float3 Eye = normalize(g_CameraPos.xyz - In.Pos);
	Out.Eye.x = dot(Eye, In.Tangent.xyz);
	Out.Eye.y = dot(Eye, In.Binormal.xyz);
	Out.Eye.z = dot(Eye, In.Normal.xyz);
	Out.Eye = normalize(Out.Eye);

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	// ���ˏ���
	float3 vReflect = reflect(normalize(In.EyeRay), float3(0, 1, 0));
	float4 FinalColor = g_Texture.Sample(g_Sampler, vReflect);

	// ���C�g����
	float3 InvLightDir = normalize(g_LightDir.xyz);
	FinalColor *= max(0.5, dot(float3(0, 1, 0), InvLightDir));

	return FinalColor
		* g_WaterTexture.Sample(g_Sampler, In.UV)
		* g_SkyTexture.Sample(g_Sampler, float2(g_LightDot.x, 0.0f));
}