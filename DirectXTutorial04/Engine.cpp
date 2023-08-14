#include "Engine.h"
#include "Matrix4f.h"
#include <d3dcompiler.h>
#include "VertexUV.h"
#include "Texture.h"
#include "Shader.h"

//#include "TextureMappingShader.h"

Engine::Engine(HINSTANCE hInstance, int width, int height, std::wstring title)
    : D3DApp(hInstance, width, height, title)
{
    //memset(&transformMatrix, 0, sizeof(transformMatrix)); //transform Buffer
}

Engine::~Engine()
{
}


bool Engine::Initialize()
{
    // â �����.
    // ��ġ ����.
    if (D3DApp::Initialize() == false)
    {
        return false;
    }

    // ��� �ʱ�ȭ.
    if (InitializeScene(device, vertexShaderBuffer) == false)
    {
        return false;
    }

    return true;
}

// 1. �޽��� ó�� ����.
// 2. ���� ����.
int Engine::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            DrawScene();
        }
    }

    return 0;
}

void Engine::Update()
{
    //transformMatrix = Matrix4f::Scale(scale) * Matrix4f::Rotation(rotation) * Matrix4f::Translation(position);
}

void Engine::DrawScene()
{
    // ��� ���� ����.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };

    // ����� (Clear) - �����δ� ������� ��ĥ�ϱ�.
    // Begin Draw(Render) - DX9.
    deviceContext->ClearRenderTargetView(renderTargetView, backgroundColor);

    // �׸��� �غ�.
    deviceContext->VSSetShader(vertexShader, NULL, NULL);
    deviceContext->PSSetShader(pixelShader, NULL, NULL);
    deviceContext->PSSetSamplers(0, 1, &samplerState); //�ȼ� ���̴� ���÷� ������Ʈ 

    // �׸���.
    RenderBuffers(deviceContext);

    // ������ �ٲٱ�. FrontBuffer <-> BackBuffer.
    swapChain->Present(1, 0);
}


bool Engine::InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer)
{
    // VertexShader ������
    HRESULT result = D3DCompileFromFile(
        L"BasicVS.hlsl",
        NULL,
        NULL,
        "main", // wchar(���ϵ� ĳ����) �ƴ϶� L �� �ٿ��� ��.
        "vs_5_0",
        NULL,
        NULL,
        &vertexShaderBuffer, // ������ �� �����ڵ带 �޾ƿ� ����.
        NULL
    );
    if (FAILED(result)) { MessageBox(nullptr, L"���� ���̴� ������ ����", L"����", 0); }

    // VS ����.
    result = device->CreateVertexShader(
        vertexShaderBuffer->GetBufferPointer(), // ���� ���� �ּ�.
        vertexShaderBuffer->GetBufferSize(), // �󸶸�ŭ ���� ��.
        nullptr,
        &vertexShader
    );
    if (FAILED(result)) { MessageBox(nullptr, L"���� ���̴� ���� ����", L"����", 0); }



    // VS�� �ϴ� ����� �Ȱ���. ���ϸ��̶� ������ ���ݾ� �ٲ�����.
    // Pixel Shader ������.
    result = D3DCompileFromFile(
        L"BasicPS.hlsl",
        NULL,
        NULL,
        "main",
        "ps_5_0",
        NULL,
        NULL,
        &pixelShaderBuffer,
        NULL
    ); 
    if (FAILED(result)) { MessageBox(nullptr, L"�ȼ� ���̴� ������ ����", L"����", 0); }

    // PS ����.
    result = device->CreatePixelShader(
        pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        nullptr,
        &pixelShader
    );
    if (FAILED(result)) { MessageBox(nullptr, L"�ȼ� ���̴� ���� ����", L"����", 0); }

    //TexturePixelShader ������ 
    //TexturePixelShader ���� 

    //pixel shader ���÷� ������Ʈ ���� 
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // ���÷� ������Ʈ ����.
    result = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(result))
    {
        MessageBox(nullptr, L"���÷� ������Ʈ ���� ����", L"����", 0);
        return false;
    }


    //Load Texture
    Texture texture;
    texture.filename = L"../res/textures/dog.png";

    if (FAILED(result))
    {
        return false;
    }
    textures.push_back(texture);


    //Texture Binding 
    for (int ix = 0; ix < textures.size(); ++ix)
    {
        deviceContext->PSSetShaderResources(
            ix,
            1,
            textures[ix].textureResource.GetAddressOf()
        );
    }


    //���⼭���� Buffer

    //1. �ﰢ�� ����� 
    // ���� ������ �����.
    // ����(Vertex) �迭.
    // �޼� ��ǥ��.
    Vertex vertices[] =
    {
        Vertex(0.0f, 0.5f, 0.5f),
        Vertex(0.5f, -0.5f, 0.5f),
        Vertex(-0.5f, -0.5f, 0.5f)
    };

    // ������ ����.
    vertexCount = ARRAYSIZE(vertices);

    // ���� ���� �����.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(vertices); // �󸶸�ŭ ������.
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    vertexBufferDesc.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // ������ ���.
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = vertices;

    // ���� ���� ����.
    result = device->CreateBuffer(
        &vertexBufferDesc,
        &vertexBufferData,
        &vertexBuffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
        return false;
    }


    //2. quad����� 
    // ���� ������ �����.
    // ����(Vertex) �迭.
    // �޼� ��ǥ��.
    Vertex vertices2[] =
    {
        Vertex(-0.5f, -0.5f, 0.5f),
        Vertex(-0.5f, 0.5f, 0.5f),
        Vertex(0.5f, 0.5f, 0.5f),
        Vertex(-0.5f, -0.5f, 0.5f),
        Vertex(0.5f, 0.5f, 0.5f),
        Vertex(0.5f, -0.5f, 0.5f)
    };

    // ������ ����.
    vertexCount2 = ARRAYSIZE(vertices2);

    // ���� ���� �����.
    D3D11_BUFFER_DESC vertexBufferDesc2;
    ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
    vertexBufferDesc2.ByteWidth = sizeof(vertices2); // �󸶸�ŭ ������.
    vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    vertexBufferDesc2.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    vertexBufferDesc2.MiscFlags = 0;
    vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;

    // ������ ���.
    D3D11_SUBRESOURCE_DATA vertexBufferData2;
    ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));
    vertexBufferData2.pSysMem = vertices2;

    // ���� ���� ����.
    HRESULT result2 = device->CreateBuffer(
        &vertexBufferDesc2,
        &vertexBufferData2,
        &vertexBuffer2
    );
    if (FAILED(result2))
    {
        MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
        return false;
    }

    //3. QuadUV ����� 
    // ���� ������ �����.
    // ����(Vertex) �迭.
    // �޼� ��ǥ��.
    VertexUV vertices3[] =
    {
        VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
        VertexUV(Vector3f(-0.5f, 0.5f, 0.5f), Vector2f(0.0f, 0.0f)),
        VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),

        VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
        VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),
        VertexUV(Vector3f(0.5f, -0.5f, 0.5f), Vector2f(1.0f, 1.0f))
    };

    // ������ ����.
    vertexCount3 = ARRAYSIZE(vertices3);

    // ���� ���� �����.
    D3D11_BUFFER_DESC vertexBufferDesc3;
    ZeroMemory(&vertexBufferDesc3, sizeof(vertexBufferDesc3));
    vertexBufferDesc3.ByteWidth = sizeof(vertices3); // �󸶸�ŭ ������.
    vertexBufferDesc3.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    vertexBufferDesc3.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    vertexBufferDesc3.MiscFlags = 0;
    vertexBufferDesc3.Usage = D3D11_USAGE_DEFAULT;

    // ������ ���.
    D3D11_SUBRESOURCE_DATA vertexBufferData3;
    ZeroMemory(&vertexBufferData3, sizeof(vertexBufferData3));
    vertexBufferData3.pSysMem = vertices3;

    // ���� ���� ����.
    HRESULT result3 = device->CreateBuffer(
        &vertexBufferDesc3,
        &vertexBufferData3,
        &vertexBuffer3
    );

    if (FAILED(result3))
    {
        MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
        return false;
    }
    

    // ���� ������ �����ϱ�.

    // ������ ���� �� ����� (�Է� ���̾ƿ�).
    //LPCSTR SemanticName;
    //UINT SemanticIndex;
    //DXGI_FORMAT Format;
    //UINT InputSlot;
    //UINT AlignedByteOffset;
    //D3D11_INPUT_CLASSIFICATION InputSlotClass;
    //UINT InstanceDataStepRate;
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        
    };

    //QuadUV
    D3D11_INPUT_ELEMENT_DESC layout2[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // �տ� position�� float 4���� 12����Ʈ�ϱ� 12.
    };

    // �Է� ���̾ƿ� ����.
    result = device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &inputLayout
    );
    if (FAILED(result)) { MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0); }

    // //QuadUV. �Է� ���̾ƿ� ����.
    result2 = device->CreateInputLayout(
        layout2,
        ARRAYSIZE(layout2),
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &inputLayout3
    );
    if (FAILED(result2)) { MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0); }




    // ��� ����(transform buffer) �����.
    D3D11_BUFFER_DESC transformBufferDesc;
    ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    transformBufferDesc.ByteWidth = sizeof(Matrix4f);
    transformBufferDesc.CPUAccessFlags = 0;
    transformBufferDesc.MiscFlags = 0;
    transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // ���� ���.
    transformMatrix = Matrix4f::Identity();

    // ������ ���.
    D3D11_SUBRESOURCE_DATA transformBufferData;
    ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    transformBufferData.pSysMem = &transformMatrix;

    // ���� ����.
    result = device->CreateBuffer(
        &transformBufferDesc,
        &transformBufferData,
        &constantBuffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
        return false;
    }

    // ��� ����2.
    if (FAILED(result2))
    {
        MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
        return false;
    }

    // ��� ����3.
    if (FAILED(result3))
    {
        MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
        return false;
    }

    return true; //������

}

void Engine::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // �׸���. (Render = Bind + Draw)
    // Bind (BindBuffer)
    unsigned int stride = sizeof(Vertex); // �ѹ��� �� ���� ���� ��. XYZ�ϱ� *3.
    unsigned int offset = 0;

    //Triangle �׸���
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Transform Buffer(��ǥ ����)
    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.5f, 0.0f,0.5f));
    
    // ��� �����Ͱ� ��� ���� ���ε�. (����.)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
    // ��� ������ ������Ʈ.
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    // Draw (DrawBuffer)
    deviceContext->Draw(vertexCount, 0); // �̰� DrawCall�̴�.

    
    //Quad �׸��� 
    // �� ��° ���� ���� ������ �Է� ���̾ƿ� ���ε�
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout2);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(-0.25f, 0.0f, -0.25f));

    // ��� ���� ���� (�� ��° ���� ���ۿ� ���� ��� ����)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    // ��� ���� ������Ʈ
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    // �׸���
    deviceContext->Draw(vertexCount2, 0);
    

    //3. QuadUV �׸��� 
    unsigned int stride2 = sizeof(VertexUV); // �ѹ��� �� ���� ���� ��.
    //unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer3, &stride2, &offset);
    deviceContext->IASetInputLayout(inputLayout3);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ���� �׸� ���� LineList.

    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.7f, 0.0f, 0.7f));


     // ��� ���� ���� (�� ��° ���� ���ۿ� ���� ��� ����)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    // ��� ���� ������Ʈ
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);


    // �׸���
    deviceContext->Draw(vertexCount3, 0);
    
}


//Mesh Getter/Setter
void Engine ::SetPosition(float x, float y, float z)
{
    position = Vector3f(x, y, z);
}

void Engine::SetPosition(Vector3f position)
{
    this->position = position;
}

void Engine::SetRotation(float x, float y, float z)
{
    rotation = Vector3f(x, y, z);
}

void Engine::SetRotation(Vector3f rotation)
{
    this->rotation = rotation;
}

void Engine::SetScale(float x, float y, float z)
{
    scale = Vector3f(x, y, z);
}

void Engine::SetScale(Vector3f scale)
{
    this->scale = scale;
}