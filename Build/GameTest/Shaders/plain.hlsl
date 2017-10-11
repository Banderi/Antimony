/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix normalMatrix;
    float4 diffuse;
    float4 camera;
};

//////////////
// TYPEDEFS //
//////////////
struct VIn
{
    float4 position : POSITION;
    float4 color : COLOR;
};
struct VOut
{
    float4 position : POSITION;
    float4 sv_position : SV_POSITION;
    float4 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VOut VShader(VIn IN)
{
	//float4 pos;
	//pos.x = position.x;
	//pos.y = position.y;
	//pos.z = position.z;
 //   pos.w = 1.0f;

    VOut OUT;

    OUT.position = mul(IN.position, worldMatrix);
    OUT.sv_position = mul(OUT.position, viewMatrix);
    OUT.sv_position = mul(OUT.sv_position, projectionMatrix);

    OUT.color = IN.color;

    return OUT;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PShader(VOut IN) : SV_TARGET
{
    float4 OUT;

    OUT = IN.color;
    OUT = OUT * diffuse.a;

    return OUT;
}