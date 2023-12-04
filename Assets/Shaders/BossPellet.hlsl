struct PSInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
};

float4 PS_Main(PSInput input) : SV_TARGET
{
	return float4(1.0, 0.2, 0.2, input.position.z);
};