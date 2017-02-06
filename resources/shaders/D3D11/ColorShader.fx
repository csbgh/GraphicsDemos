//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Col : COLOR;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

PS_INPUT VS(VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = input.Pos;
	output.Col = input.Col;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader float4 Pos : SV_POSITION 
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT input) : SV_Target
{
    return input.Col;    // Yellow, with Alpha = 1
}