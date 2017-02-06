//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

cbuffer perFrameUniforms : register(b0)
{
	float4x4 viewProjection;
	float4x4 uiOrthoProjection;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR;
};

PS_INPUT VS(VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(viewProjection, input.Pos);
	output.Tex = input.Tex;
	output.Col = input.Col;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader float4 Pos : SV_POSITION 
//--------------------------------------------------------------------------------------

Texture2D<float4> Tex : register(t0);
SamplerState Sam : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
    return Tex.Sample(Sam, input.Tex) * input.Col;    // Yellow, with Alpha = 1
}