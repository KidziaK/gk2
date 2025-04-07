cbuffer cbSurfaceColor : register(b0) //Pixel Shader constant buffer slot 0
{
    float4 surfaceColor;
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
    //TODO : 1.32. Calculate billboard pixel color
    float d = length(i.tex);
    float3 color = max(0, 1.0f - 2 * d);
    return float4(surfaceColor.xyz * color, 1.0f);
}
