cbuffer MatrixBuffer : register(b0)
{
    matrix mWorld;
    matrix mProjection;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 textcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

//Expand the float3 into a float4
    float4 pos = float4(input.position, 1.0f);

//Apply transformations
    pos = mul(pos, mWorld);
    pos = mul(pos, mProjection);

    output.position = pos;
    output.texcoord = input.texcoord;
    return output;
}
