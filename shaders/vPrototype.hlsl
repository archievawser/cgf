struct PSInput 
{ 
	float4 Pos   : SV_POSITION; 
};

cbuffer WorldData
{
	float4x4 mvp;
};

void Main(in float2 position : POSITION,
		  out PSInput PSIn) 
{
	float4 xpos = float4(position, 0.0, 1.0);
	PSIn.Pos = mul(mvp, xpos);
}