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
	float2 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR0;
};

PS_INPUT VS(VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(uiOrthoProjection, float4(input.Pos.xy, 0.f, 1.f));
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
    return Tex.Sample(Sam, input.Tex) * input.Col;
}