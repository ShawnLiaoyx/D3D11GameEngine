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
    float4 reNormal = NormalTexture.Sample(DefaultSampler, pIn.uv);
    float3 l_final = c_ambient;
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
    float4 worldPos = WorldPosTexture.Sample(DefaultSampler, pIn.uv);

    int skipLighting = !ceil(diffuse.a);

    for (int i = skipLighting * MAX_POINT_LIGHTS; i < MAX_POINT_LIGHTS; i++)
    {
        if (c_pointLight[i].isEnabled) {
            float4 fromLight = float4(c_pointLight[i].position, 1.0) - worldPos;
            float4 l = normalize(fromLight);
            float4 r = reflect(-l, reNormal);
            float4 v = normalize(float4(c_cameraPosition, 1.0) - worldPos);
            float falloff = 1 - smoothstep(c_pointLight[i].innerRadius, c_pointLight[i].outerRadius, length(fromLight));
            l_final += c_pointLight[i].lightColor * max(dot(l, reNormal), 0.0) * falloff;
            l_final += c_pointLight[i].lightColor * max(pow(dot(r, v), diffuse.a), 0.0) * falloff;
        }
    }
    float4 colorFinal = !skipLighting * diffuse * float4(l_final, 0.0) + skipLighting * diffuse;
    return colorFinal;
}