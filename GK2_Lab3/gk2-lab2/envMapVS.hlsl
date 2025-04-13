cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0
{
    matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1
{
    matrix viewMatrix;
    matrix invViewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2
{
    matrix projMatrix;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 norm : NORMAL0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 tex : TEXCOORD0;
};

PSInput main(VSInput i)
{
    PSInput o = (PSInput) 0;
    float4 worldPosition = float4(i.pos, 1.0f);
    float4 worldNormal = float4(i.norm, 0.0f);
    
    worldPosition = mul(worldMatrix, worldPosition);
    worldNormal = normalize(mul(worldMatrix, worldNormal));
    o.pos = worldPosition;
    

	// TODO : 1.23 Calculate cube map texture coordinates
    float3 camPos = mul(invViewMatrix, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    float3 worldView = normalize(o.pos.xyz - camPos);
    o.tex = reflect(worldView, worldNormal.xyz);

    o.pos = mul(viewMatrix, o.pos);
    o.pos = mul(projMatrix, o.pos);

    return o;
}