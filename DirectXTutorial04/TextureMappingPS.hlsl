// �ȼ� ���̴� �Է�.
struct ps_input
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD;
};

// �ؽ��� ������.
Texture2D image;
SamplerState samplerState;

//�ȼ� ���̴�.
//float4 main(float4 position : POSITION) : SV_TARGET

float4 main(ps_input input) : SV_TARGET
{
    // �̹����� ���� �����ϱ�.
    float4 color = image.Sample(samplerState, input.texCoord);
    
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return color;
}

//ps_input main(float4 position : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
//{
//    ps_input output;
//    output.position = position;
//    output.texCoord = texCoord;
    
//    return output;
//}
