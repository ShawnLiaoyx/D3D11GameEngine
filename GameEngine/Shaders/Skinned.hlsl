#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    uint4 bone : BONE0;
    float4 weight : WEIGHT0;
    float2 uv : UV0;
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

    float4 v = float4(vIn.position, 1.0);
    output.worldPosition = vIn.weight.x * mul(v, c_skinMatrix[vIn.bone.x]);
    output.worldPosition += vIn.weight.y * mul(v, c_skinMatrix[vIn.bone.y]);
    output.worldPosition += vIn.weight.z * mul(v, c_skinMatrix[vIn.bone.z]);
    output.worldPosition += vIn.weight.w * mul(v, c_skinMatrix[vIn.bone.w]);

    output.worldPosition = mul(output.worldPosition, c_modelToWorld);
    output.position = mul(output.worldPosition, c_viewProj);
    output.uv = vIn.uv;

    float4 n = float4(vIn.normal, 0.0);
    output.normal = vIn.weight.x * mul(n, c_skinMatrix[vIn.bone.x]);
    output.normal += vIn.weight.y * mul(n, c_skinMatrix[vIn.bone.y]);
    output.normal += vIn.weight.z * mul(n, c_skinMatrix[vIn.bone.z]);
    output.normal += vIn.weight.w * mul(n, c_skinMatrix[vIn.bone.w]);

    output.normal = mul(output.normal, c_modelToWorld);
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

    float4 colorFinal = float4(DiffuseTexture.Sample(DefaultSampler, pIn.uv) * c_diffuseColor, 
        c_specularPower);

    POut output;
    output.diffuse = colorFinal;
    output.normal = float4(reNormal, 0.0f);
    output.worldPos = float4(worldPos, 1.0f);
    return output;
}