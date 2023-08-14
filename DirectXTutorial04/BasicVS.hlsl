// ��� ������.
// Constant Buffer. (��� ����.)
cbuffer transform : register(b0)
{
	// 4x4 ���. 
	// float4x4, float3x3, float2x2
    matrix world;
};

// ���� �Է�.
struct vs_input
{
    float4 position : POSITION;
};

// ���� ���.
struct vs_output
{
    float4 position : SV_POSITION;
};

// float4 main(float4 position : POSITION) : SV_POSITION
// float4 main(vs_input input) : SV_POSITION
vs_output main(vs_input input)
{
    vs_output output;
    output.position = input.position;
    output.position = mul(input.position, world);

    return output;
}