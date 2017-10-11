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

//////////////
// TYPEDEFS //
//////////////
struct VOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
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

    output.normal = normal;
    output.uv = uv;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PShader(float4 position : SV_POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET
{
    float4 output;

    output = diffuse;

    return output;
}