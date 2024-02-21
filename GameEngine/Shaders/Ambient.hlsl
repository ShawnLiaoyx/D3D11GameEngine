#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float2 uv : UV0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.position = float4(vIn.position, 1.0);
    output.uv = vIn.uv;
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float3 l_final = c_ambient;
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);

    int skipLighting = !ceil(diffuse.a);

    float4 colorFinal = !skipLighting * diffuse * float4(l_final, 0.0) + skipLighting * diffuse;
    return colorFinal;
}