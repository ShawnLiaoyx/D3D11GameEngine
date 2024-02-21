#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float2 uv : UV0;
    float3 normal : NORMAL0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
    float4 normal : NORMAL0;
    float4 worldPosition : WORLD_POSITION;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPosition, c_viewProj);
    output.uv = vIn.uv;
    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    return output;
}

struct POut
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 worldPos : SV_TARGET2;
};

POut PS(VOut pIn)
{
    float3 reNormal = normalize(pIn.normal).xyz;
    float3 worldPos = pIn.worldPosition.xyz;

    float4 colorFinal = DiffuseTexture.Sample(DefaultSampler, pIn.uv);

    POut output;
    output.diffuse = float4(colorFinal.rgb, 0.0f);
    output.normal = float4(reNormal, 0.0f);
    output.worldPos = float4(worldPos, 1.0f);
    return output;
}