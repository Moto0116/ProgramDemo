
struct ParticleData
{
	float4 Pos;
	float4 Scale;
	float4 Color;
	float4 Life;
};

RWStructuredBuffer<ParticleData> DataBuffer : register(u0);

struct CSInput
{
	uint3   GroupThread  : SV_GroupThreadID;
	uint3   Group        : SV_GroupID;
	uint    GroupIndex   : SV_GroupIndex;
	uint3   Dispatch     : SV_DispatchThreadID;
};

[numthreads(600, 1, 1)]
void CS(const CSInput In)
{
	uint Index = In.GroupThread.x;

	DataBuffer[Index].Pos;

}
