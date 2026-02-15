struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 WorldPos   : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
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


Texture2D    g_Texture;
SamplerState g_Texture_sampler;


void ProcessVertex(
	in float3 position : POSITION, 
	in float3 normal : NORMAL,
	in float2 uv : TEXCOORD,
	out PSInput PSIn)
{
	float4 xpos = float4(position, 1.0);
	PSIn.Pos = mul(ModelViewProjection, xpos);
	PSIn.WorldPos = position;
	PSIn.Normal = mul(Model, normal);
	PSIn.UV = uv;
}


void ProcessFragment(in PSInput PSIn, out PSOutput PSOut)
{
	float3 normal = PSIn.Normal;
	float3 lightDir = float3(1, 1, 1);
	float intensity = max(0.01, dot(normal, lightDir) / pow(distance(PSIn.WorldPos, 2 * -(lightDir)), 2));

    PSOut.Color = g_Texture.Sample(g_Texture_sampler, PSIn.UV);
}	