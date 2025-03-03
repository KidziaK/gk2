cbuffer transformations : register(b0) {
    matrix MVP;
};

struct VSIn
{
    float3 pos : POSITION;
    float3 col : COLOR;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VSOut main( VSIn i )
{
    VSOut o;
    o.pos = mul(MVP, float4(i.pos, 1.0f));
    o.col = float4(i.col, 1.0f);
    return o;
}