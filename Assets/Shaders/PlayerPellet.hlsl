struct PSInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
};

float4 PS_Main(PSInput input) : SV_TARGET
{
	return float4(0.2, 0.2, 1.0, input.position.z);
};