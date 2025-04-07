struct Light
{
    float4 position;
    float4 color;
};

struct Lighting
{
    float4 ambient;
    float4 surface;
    Light lights[3];
};

cbuffer cbSurfaceColor : register(b0) //Pixel Shader constant buffer slot 0 - matches slot in psBilboard.hlsl
{
    float4 surfaceColor;
}

cbuffer cbLighting : register(b1) //Pixel Shader constant buffer slot 1
{
    Lighting lighting;
}

//TODO : 0.8. Modify pixel shader input structure to match vertex shader output
struct PSInput
{
    float4 pos : SV_POSITION;
    float3 N : NORMAL;
    float3 V : VIEW;
    float3 P : POSITION_WORLD;
};

float4 main(PSInput i) : SV_TARGET
{
    //TODO : 0.9. Calculate output color using Phong Illumination Model
    float3 V = i.V;
    float3 N = i.N;
    float3 P = i.P;
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float specular_alpha = 0.0f;
    
    for (int k = 0; k < 3; k++)
    {
        Light l = lighting.lights[k];
        if (l.color.w != 0)
        {
            float3 L = normalize(l.position.xyz - P);
            float3 H = normalize(V + L);
            
            color += l.color.xyz * surfaceColor.xyz * lighting.surface.y * clamp(dot(N, L), 0.0f, 1.0f);
            
            float NdH = dot(N, H);
            NdH = clamp(NdH, 0.0f, 1.0);
            NdH = pow(NdH, lighting.surface.w);
            NdH *= lighting.surface.z;
            
            specular_alpha += NdH;
            color += l.color.xyz * NdH;
        }
    }

    float3 ambient = lighting.ambient.xyz * lighting.surface.x;
    return saturate(float4(ambient + color, surfaceColor.w + specular_alpha));
    // return saturate(float4(dot(N, V), dot(N, V), dot(N, V), dot(N, V)));

}