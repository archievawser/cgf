struct PSInput
{
    float4 Pos   : SV_POSITION;
};


struct PSOutput
{
    float4 Color : SV_TARGET;
};


cbuffer WorldData
{
	float4x4 mvp;
};


void ProcessVertex(in float2 position : POSITION, out PSInput PSIn)
{
	float4 xpos = float4(position, 0.0, 1.0);
	PSIn.Pos = mul(mvp, xpos);
}


void ProcessFragment(in PSInput PSIn, out PSOutput PSOut)
{
    PSOut.Color = float4(1.0, 1.0, 1.0, 1.0);
}