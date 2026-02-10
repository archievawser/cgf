struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 WorldPos   : POSITION;
    float3 Normal : NORMAL;
};


struct PSOutput
{
    float4 Color : SV_TARGET;
};


cbuffer ShaderCommon
{
	float4x4 ModelViewProjection;
	float4x4 Model;
};


void ProcessVertex(
	in float3 position : POSITION, 
	in float3 normal : NORMAL,
	out PSInput PSIn)
{
	float4 xpos = float4(position, 1.0);
	PSIn.Pos = mul(ModelViewProjection, xpos);
	PSIn.WorldPos = position;
	PSIn.Normal = mul(Model, normal);
}


void ProcessFragment(in PSInput PSIn, out PSOutput PSOut)
{
	float3 normal = PSIn.Normal;
	float3 lightDir = float3(1, 1, 1);
	float intensity = max(0.01, dot(normal, lightDir) / pow(distance(PSIn.WorldPos, 2 * -(lightDir)), 2));

    PSOut.Color = float4(float3(intensity, intensity, intensity), 1.0);
}