Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer model : register(b0)
{
	matrix g_World;
};

cbuffer CubeMapData : register(b5)
{
	matrix g_View[6];
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
	float4 Pos	  : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV     : TEXCOORD;
};

struct GS_OUTPUT
{
	float4 Pos	  : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV     : TEXCOORD;
	uint RTIndex  : SV_RenderTargetArrayIndex;
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Pos = mul(float4(In.Pos, 1.0f), g_World);
	Out.Normal = In.Normal;
	Out.UV = In.UV;

	return Out;
}

// 最大出力頂点数
[maxvertexcount(18)]
void GS(triangle VS_OUTPUT In[3], inout TriangleStream<GS_OUTPUT> TriStream)
{
	for (int i = 0; i < 6; i++)
	{
		GS_OUTPUT Out;
		Out.RTIndex = i;
		for (int j = 0; j < 3; j++)
		{
			Out.Pos = mul(In[j].Pos, g_View[i]);
			Out.Pos = mul(Out.Pos, g_Proj);
			Out.Normal = In[j].Normal;
			Out.UV = In[j].UV;
			TriStream.Append(Out);	// 追加
		}
		TriStream.RestartStrip();	// 次のプリミティブに移る
	}
}

float4 PS(GS_OUTPUT In) : SV_Target
{
	return g_Texture.Sample(g_Sampler, In.UV);
}
