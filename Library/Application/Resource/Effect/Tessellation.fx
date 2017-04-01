Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer global : register(b0)
{
	matrix g_WorldMat;
	float4 g_ViewPort;
};

cbuffer TessellationData : register(b4)
{
	float4 g_TessFactor   : SV_TessFactor;
	float4 g_InsideFactor : SV_InsideTessFactor;
};

struct VS_INPUT
{
	float4 Pos   : SV_POSITION;
	float2 UV    : TEXCOORD;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Pos     : SV_POSITION;
	float2 UV     : TEXCOORD;
	float4 Color : COLOR;
};

struct ConstantHS_OUTPUT
{
	float tessFactor[4] : SV_TessFactor;		// 形状により配列サイズを変動させる必要がある
	float insideFactor[2] : SV_InsideTessFactor;
};


VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.Pos = mul(In.Pos, g_WorldMat);
	Out.Pos.x = (Out.Pos.x / g_ViewPort.x) * 2 - 1;
	Out.Pos.y = 1 - (Out.Pos.y / g_ViewPort.y) * 2;
	Out.UV = In.UV;
	Out.Color = In.Color;

	return Out;
}



ConstantHS_OUTPUT ConstantHS(InputPatch<VS_OUTPUT, 4> Patch)
{
	ConstantHS_OUTPUT Out;

	float4 retf;
	float2 ritf, uitf;
	Process2DQuadTessFactorsAvg(float4(3.0f, 3.0f, 3.0f, 3.0f), float2(1.0f, 1.0f), retf, ritf, uitf);

	Out.tessFactor[0] = retf.x;
	Out.tessFactor[1] = retf.y;
	Out.tessFactor[2] = retf.z;
	Out.tessFactor[3] = retf.w;
	Out.insideFactor[0] = ritf.x;
	Out.insideFactor[1] = ritf.y;

	return Out;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
VS_OUTPUT HS(InputPatch<VS_OUTPUT, 4> Patch, uint PointID : SV_OutputControlPointID)
{
	return Patch[PointID];
}

[domain("quad")]
VS_OUTPUT DS(
	ConstantHS_OUTPUT InConst, 
	float2 DomainLocation : SV_DomainLocation, 
	const OutputPatch<VS_OUTPUT, 4> Patch)
{
	VS_OUTPUT Out;

	float2 Top = lerp(Patch[0].Pos.xy, Patch[1].Pos.xy, DomainLocation.x);		// topを算出
	float2 Bottom = lerp(Patch[2].Pos.xy, Patch[3].Pos.xy, DomainLocation.x);	// bottomを算出
	float2 Center = lerp(Top, Bottom, DomainLocation.y);						// topとbottomから中心を算出

	Out.Pos = float4(Center, 0.0f, 1.0f);	// 2dなのでzは0にする

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float4 OutColor;
	OutColor = g_Texture.Sample(g_Sampler, In.UV);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
