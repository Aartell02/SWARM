cbuffer TransformBuffer : register(b0)
{
    float4x4 worldViewProj;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 worldPos = float4(input.position, 1.0f);
    output.position = mul(worldPos, worldViewProj);
    output.normal = input.normal;
    output.color = input.color;

    return output;
}
