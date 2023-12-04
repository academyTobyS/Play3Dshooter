// Provided by the framework.
cbuffer DrawConstantData : register(b1)
{
	float4x4 mvpMtx;
	float4x4 worldMtx;
	float4x4 normalMtx;
};

struct VSInput
{
	float3 position : POSITION;
	float4 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};


PSInput VS_Main(VSInput input)
{
	PSInput output;

	output.position = mul(mvpMtx, float4(input.position.xyz, 1.0f));
	output.normal = mul(normalMtx, float4(input.normal.xyz, 0.0f));

	return output;
}

float4 PS_Main(PSInput input) : SV_TARGET
{
	return float4(0.2, 0.2, 1.0, input.position.z);
};