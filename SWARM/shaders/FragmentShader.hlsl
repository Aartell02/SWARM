struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Simple lighting: use normal dot light direction
    float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f));
    float lighting = max(dot(input.normal, lightDir), 0.2f);

    return input.color * float4(lighting, lighting, lighting, 1.0f);
}
