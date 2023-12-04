// Provided by the framework.
cbuffer FrameConstantData : register(b0)
{
	float4x4 viewMtx;
	float4x4 projectionMtx;
	float4x4 viewProjectionMtx;
	float4 viewPosition;
	float4 time;
};

// Provided by the framework.
cbuffer DrawConstantData : register(b1)
{
	float4x4 mvpMtx;
	float4x4 worldMtx;
	float4x4 normalMtx;
};

// Any data (must match material const buffer)
cbuffer MaterialConstantData : register(b3)
{
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
	float4 position : SV_POSITION; //<< This attribute must exist.
	float3 normal : NORMAL;
	float2 uv : UV;
};

PSInput VS_Main(VSInput input)
{
	PSInput output;

	output.position = mul(mvpMtx, float4(input.position.xyz, 1.0f));
	output.normal = mul(normalMtx, float4(input.normal.xyz, 0.0f));
	output.uv = input.uv;

	return output;
}

Texture2D g_diffuseTexture : register(t0);
SamplerState g_diffuseSampler : register(s0);

float4 PS_Main(PSInput input) : SV_TARGET
{
	float4 col = g_diffuseTexture.Sample(g_diffuseSampler, input.uv);
	if(col.a < 0.5)
	{
		// no need to enable transparency, simply discarding transparent pixels
		discard;
	}
	return float4(col.r, col.g, col.b, col.a);
}