cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0 - matches slot in vsBilboard.hlsl
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1 - matches slot in vsBilboard.hlsl
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2 - matches slot in vsBilboard.hlsl
{
	matrix projMatrix;
};

//TODO : 0.4. Change vertex shader input structure to match new vertex type
struct VSInput
{
	float3 pos : POSITION;
    float3 normal : NORMAL;
};

//TODO : 0.5. Change vertex shader output structure to store position, normal and view vectors in global coordinates
struct PSInput
{
	float4 pos : SV_POSITION;
    float3 N : NORMAL;
    float3 V : VIEW;
    float3 P : POSITION_WORLD;
};
PSInput main(VSInput i)
{
	PSInput o;
	float4 pos = mul(worldMatrix, float4(i.pos, 1.0f));
    o.P = pos.xyz;
	//TODO : 0.6. Store global (world) position in the output

    pos = mul(viewMatrix, pos);
	o.pos = mul(projMatrix, pos);

	//TODO : 0.7. Calculate output normal and view vectors in global coordinates frame
	//Hint: you can calculate camera position from inverted view matrix
		
    
    o.N = normalize(mul(worldMatrix, float4(i.normal, 0.0f)).xyz);
    float3 cameraPosition = mul(invViewMatrix, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    o.V = normalize(cameraPosition - o.P);
		
	return o;
}