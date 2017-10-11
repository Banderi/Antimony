/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	float4 diffuse;
};
BlendState AlphaBlendingOn
{
	BlendEnable[0] = TRUE;
	DestBlend = INV_SRC_ALPHA;
	SrcBlend = SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	float4 pos;
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
    pos.w = 1.0f;

    VOut output;

    output.position = mul(position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.color = color * diffuse;
	output.color.a = color.a;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	float4 output;
	output = color; // start from vertex color

	if (position.w <= 0)
	{
		output = 0x00000000;
	}
	else
	{
		//output *= (1 / position.w);
	}

	/*output.r = position.x;
	output.g = position.y;
	output.b = position.z;
	output *= 0.01;*/

	output.a = color.a;

    return output;
}