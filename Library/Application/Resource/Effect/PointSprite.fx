Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer camera : register(b1)
{
	matrix g_View;
	matrix g_Proj;
	float4 g_CameraPos;
	float4 g_CameraDir;
	float4 g_Aspect;
};

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD;
	float4 Color    : COLOR;
	float4x4 mat    : MATRIX;          // インスタンスごとに設定される行列
	float3 Position	: POS;
	uint InstanceId : SV_InstanceID;   // インスタンスＩＤ
};

struct VS_OUTPUT
{
	float4 PosWVP   : SV_POSITION;
	float2 UV		: TEXCOORD;
	float4 Color    : COLOR;
	float3 Position	: POS;
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	float4x4 Mat = mul(In.mat, g_View);
	Mat = mul(Mat, g_Proj);
	Out.PosWVP = mul(float4(In.Pos, 1.0f), Mat);
	Out.UV = In.UV;
	Out.Color = In.Color;
	Out.Position = In.Position;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float4 TextureColor = float4(1, 1, 1, 1);
	if (In.Position.y <= 1)
	{
		TextureColor = g_Texture.Sample(g_Sampler, In.UV);
	}
	return In.Color * TextureColor;
}

///@todo 雨はジオメトリを使用して描画する
// 上に行くほど雨も小さくしていく感じに
