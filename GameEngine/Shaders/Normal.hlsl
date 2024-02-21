#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float2 uv : UV0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
    float4 normal : NORMAL0;
    float4 tangent : TANGENT0;
    float4 worldPosition : WORLD_POSITION;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPosition, c_viewProj);
    output.uv = vIn.uv;
    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    output.tangent = mul(float4(vIn.tangent, 0.0), c_modelToWorld);
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

    float3 pNormal = normalize(pIn.normal.xyz);
    float3 pTangent = normalize(pIn.tangent.xyz);
    float3 pBinormal = cross(pNormal, pTangent);
    float3x3 TBN;
    TBN._m00_m01_m02 = pTangent;
    TBN._m10_m11_m12 = pBinormal;
    TBN._m20_m21_m22 = pNormal;
    float3 sampleNormal = NormalTexture.Sample(DefaultSampler, pIn.uv) * 2 - float3(1.0, 1.0, 1.0);
    float3 reNormal = mul(sampleNormal, TBN);
    float3 worldPos = pIn.worldPosition.xyz;

    float4 colorFinal = float4(DiffuseTexture.Sample(DefaultSampler, pIn.uv) * c_diffuseColor,
        c_specularPower);

    POut output;
    output.diffuse = colorFinal;
    output.normal = float4(reNormal, 0.0f);
    output.worldPos = float4(worldPos, 1.0f);
    return output;
}