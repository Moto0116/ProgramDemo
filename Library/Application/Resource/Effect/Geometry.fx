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
	float4 g_FrameTime;
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
	float4 Pos     : SV_POSITION;
	float4 Normal  : NORMAL;
	float2 UV      : TEXCOORD;
	float4 LightUV : TEXCOORD2;
	float4 Color   : COLOR;
};



VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Pos = float4(In.Pos, 1.0f);
	Out.Normal = float4(In.Normal, 1.0f);
	Out.UV = In.UV;

	// ライトビュープロジェクション
	float4x4 LightMat = mul(g_World, g_LightView);
	LightMat = mul(LightMat, g_LightProj);
	Out.LightUV = mul(float4(In.Pos, 1.0f), LightMat);

	// 法線とライトからカラー値を計算
	float3 InvLightDir = normalize(g_LightDir.xyz);
	float3 Normal = normalize(In.Normal.xyz);
	Out.Color = max(g_Ambient, dot(Normal, InvLightDir));

	return Out;
}

[maxvertexcount(9)]   
void GS(triangle VS_OUTPUT In[3], inout TriangleStream<VS_OUTPUT> TriStream)
{
	VS_OUTPUT Out;

	float4x4 Mat = mul(g_World, g_View);
	Mat = mul(Mat, g_Proj);

	for (int i = 0; i < 3; i++)
	{
		/*
		float3 V = In[i].Normal.xyz;
		float T = g_FrameTime.x;
		float G = 0.5f;
		float3 NextPos = In[i].Pos.xyz + ((V * T) + (0.5 * G * (T * T)));
		*/

		Out.Pos = mul(float4(In[i].Pos.xyz, 1.0f), Mat);
		Out.Normal = In[i].Normal;
		Out.UV = In[i].UV;
		Out.LightUV = In[i].LightUV;
		Out.Color = In[i].Color;

		TriStream.Append(Out);
	}
	
	TriStream.RestartStrip();
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float4 ShadowColor = In.Color;
	float ZValue = In.LightUV.z / In.LightUV.w;

	float2 Texel;
	Texel.x = (In.LightUV.x / In.LightUV.w) * 0.5f + 0.5f;
	Texel.y = (In.LightUV.y / In.LightUV.w) * -0.5f + 0.5f;
	float SM_Z = g_DepthTexture.Sample(g_Sampler, Texel).r;

	if (Texel.x <= 0.01 || Texel.x >= 0.99 || Texel.y <= 0.01 || Texel.y >= 0.99)
	{
	}
	else if (ZValue > (SM_Z +0.00002f))
	{
		ShadowColor.rgb = In.Color.rgb * 0.4f;
	}

	return g_Texture.Sample(g_Sampler, In.UV) * ShadowColor;
}