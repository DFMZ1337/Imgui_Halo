/*
 * Halo Pixel Shader (HLSL)
 * Compile for Shader Model 5.0
 */

cbuffer HaloCB : register(b0)
{
    float4 g_Color;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 PS(PSInput input) : SV_TARGET
{
    float4 texColor = g_Texture.Sample(g_Sampler, input.TexCoord);
    texColor.a *= g_Color.a;
    return texColor * g_Color;
}

// ------------------------------
// Compiled Shader (SM 5.0)
// ------------------------------

static const BYTE HaloPS_Bytecode[] = {
    /*
     * Shader Model 5.0 compiled bytecode will go here
     * For now, we'll use a simple implementation
     */
    
    // Simple hardcoded pixel shader bytecode
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
