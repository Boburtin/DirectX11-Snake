struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 PSMain(VS_OUT input) : SV_Target
{
    return input.color;
}
