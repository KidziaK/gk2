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
	float4 normal : NORMAL;
    float4 view : VIEW;
    float4 posWorld : POS;
};

float4 main(PSInput i) : SV_TARGET
{
	//TODO : 0.9. Calculate output color using Phong Illumination Model
    float4 lightsContribution = float4(0.0, 0.0, 0.0, 1.0);
    
    for (int j = 0; j < 3; ++j)
    {
        float4 lightPosition = lighting.lights[j].position;
        float4 L = lightPosition - i.posWorld;
        float distance = length(L);
        L = normalize(L);
        float distance2 = distance * distance;
		
        float4 kd = lighting.surface.y * surfaceColor;
        float ks = lighting.surface.z;
        float m = lighting.surface.w;
		
        float NdL = max(0, dot(i.normal, L));
		
        float4 R = reflect(L, i.normal);
		
        float RdV = max(0, dot(R, i.view));
        
        float4 lightColor = lighting.lights[j].color;
		
        lightsContribution += lightColor * (kd * NdL + ks * pow(RdV, m));

    }
    
    float ka = lighting.surface.x;
	
    return saturate(lighting.ambient * ka + lightsContribution);
}