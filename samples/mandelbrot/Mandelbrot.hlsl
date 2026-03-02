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


struct complex
{
	float real;
	float imag;
};


complex complex_add(complex a, complex b)
{
	complex result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}


complex complex_add(int a, complex b)
{
	complex result;
	result.real = a + b.real;
	result.imag = b.imag;
	return result;
}


complex complex_mul(complex a, complex b)
{
	complex result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}


float complex_abs(complex n)
{
	return sqrt(n.real * n.real + n.imag * n.imag);
}


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
	float4 black = float4(0, 0, 0, 1);
	float4 white = float4(1, 1, 1, 1);

	complex z;
	z.real = 0;
	z.imag = 0;
	
	float2 plane = float2(PSIn.UV.x - 0.5, PSIn.UV.y - 0.5) * 5;

	complex coord;
	coord.real = plane.x;
	coord.imag = plane.y;

	int i = 0;

	for(; i < 512; i++)
	{
		z = complex_add(complex_mul(z, z), coord);

		if(complex_abs(z) > 2) break;
	}

    PSOut.Color = complex_abs(z) < 2 ? black : (white / 200 * i);
}