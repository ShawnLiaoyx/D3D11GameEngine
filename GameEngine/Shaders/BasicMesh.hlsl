#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float4 color : COLOR0;
    float2 uv : UV0;
    float3 normal : NORMAL0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : UV0;
    float4 normal : NORMAL0;
    float4 worldPosition : WORLD_POSITION;
};

VOut VS(VIn vIn)
{
    VOut output;

    output.worldPosition = mul(float4(vIn.position, 1.0), c_modelToWorld);
    output.position = mul(output.worldPosition, c_viewProj);
    output.color = vIn.color;
    output.uv = vIn.uv;
    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 reNormal = normalize(pIn.normal);
    float3 l_final = c_ambient;
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        if (c_pointLight[i].isEnabled) {
            float4 fromLight = float4(c_pointLight[i].position, 1.0) - pIn.worldPosition;
            float4 l = normalize(fromLight);
            float4 r = reflect(-l, reNormal);
            float4 v = normalize(float4(c_cameraPosition, 1.0) - pIn.worldPosition);
            float falloff = 1 - smoothstep(c_pointLight[i].innerRadius, c_pointLight[i].outerRadius, length(fromLight));
            l_final += c_pointLight[i].lightColor * max(dot(l, reNormal), 0.0) * falloff;
            l_final += c_pointLight[i].lightColor * max(pow(dot(r, v), c_specularPower), 0.0) * falloff;
        }
    }
    float4 colorFinal = DiffuseTexture.Sample(DefaultSampler, pIn.uv) * pIn.color * float4(l_final, 0.0);
    return colorFinal;
}