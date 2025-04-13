cbuffer cbProj : register(b0) //Geometry Shader constant buffer slot 0
{
	matrix projMatrix;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex1: TEXCOORD0;
	float2 tex2: TEXCOORD1;
};

static const float TimeToLive = 4.0f;

[maxvertexcount(4)]
void main(point GSInput inArray[1], inout TriangleStream<PSInput> ostream)
{
	GSInput i = inArray[0];
	float sina, cosa;
	sincos(i.angle, sina, cosa);
	float dx = (cosa - sina) * 0.5 * i.size;
	float dy = (cosa + sina) * 0.5 * i.size;
	
	// TODO : 1.30 Initialize 4 vertices to make a bilboard and append them to the ostream
    PSInput bottom_left = (PSInput) 0;
    bottom_left.pos = i.pos + float4(-dx, -dy, 0.0f, 0.0f);
    bottom_left.pos = mul(projMatrix, bottom_left.pos);
    bottom_left.tex1 = float2(0.0, 1.0);
    bottom_left.tex2 = float2(i.age / TimeToLive, 0.5f);
	
    PSInput top_left = (PSInput) 0;
    top_left.pos = i.pos + float4(-dy, dx, 0.0f, 0.0f);
    top_left.pos = mul(projMatrix, top_left.pos);
    top_left.tex1 = float2(0.0, 0.0);
    top_left.tex2 = float2(i.age / TimeToLive, 0.5f);
	
    PSInput bottom_right = (PSInput) 0;
    bottom_right.pos = i.pos + float4(dy, -dx, 0.0f, 0.0f);
    bottom_right.pos = mul(projMatrix, bottom_right.pos);
    bottom_right.tex1 = float2(1.0, 1.0);
    bottom_right.tex2 = float2(i.age / TimeToLive, 0.5f);
	
    PSInput top_right = (PSInput) 0;
    top_right.pos = i.pos + float4(dx, dy, 0.0f, 0.0f);
    top_right.pos = mul(projMatrix, top_right.pos);
    top_right.tex1 = float2(1.0, 0.0);
    top_right.tex2 = float2(i.age / TimeToLive, 0.5f);
	
    ostream.Append(bottom_left);
    ostream.Append(top_left);
    ostream.Append(bottom_right);
    ostream.Append(top_right);

	ostream.RestartStrip();
}