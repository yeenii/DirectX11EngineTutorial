#include "Engine.h"
#include "Matrix4f.h"
#include <d3dcompiler.h>
#include "VertexUV.h"
#include "Texture.h"
#include "Shader.h"
#include "InputProcessor.h"
#include "ResourceLoader.h"
#include <string>
//#include "TextureMappingShader.h"

Engine::Engine(HINSTANCE hInstance, int width, int height, std::wstring title)
    : D3DApp(hInstance, width, height, title), width(width), height(height), vertices(), indices()
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
            //// ESC ����.
            //if (InputProcessor::IsKeyDown(Keyboard::Keys::Escape) == true)
            //{
            //    if (MessageBox(nullptr, L"�����Ͻðڽ��ϱ�?", L"����", MB_YESNO) == IDYES)
            //    {
            //        DestroyWindow(Window::WindowHandle());
            //        return 0;
            //    }
            //}

            Update();
            DrawScene();
        }
    }

    return 0;
}

void Engine::Update()
{
    //������Ʈ�� �� ���� ������ �� �־��ֱ� 
    static float t = 0;
    //t += 0.2;
    //transformMatrix = Matrix4f::Scale(scale) * Matrix4f::Rotation(rotation) * Matrix4f::Translation(position);
    transformMatrix.mWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(1.f, 1.f, 1.f), DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f)), DirectX::XMMatrixTranslation(t, t, t)));
    
    
    //// �׽�Ʈ ��.
    //static float xPos = quad.Position().x;
    //if (InputProcessor::IsKeyDown(Keyboard::Keys::A))
    //{
    //    xPos -= 0.01f;
    //}
    //if (InputProcessor::IsKeyDown(Keyboard::Keys::D))
    //{
    //    xPos += 0.01f;
    //}
    //quad.SetPosition(xPos, quad.Position().y, 0.0f);
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

    //deviceContext->VSSetShader(TVertexShader, NULL, NULL);
    //deviceContext->PSSetShader(TPixelShader, NULL, NULL);
    //deviceContext->PSSetSamplers(0, 1, &samplerState); //�ȼ� ���̴� ���÷� ������Ʈ 

    //�׸��� �غ� 
    //TextureMappingShader 
    //deviceContext->VSSetShader(vertexShader, NULL, NULL); //Vertexshader
    //deviceContext->PSSetShader(pixelShader, NULL, NULL); //pixelshader
    //for (int ix = 0; ix < textures.size(); ++ix)
    //{
    //    deviceContext->PSSetShaderResources(
    //        ix,
    //        1,
    //        textures[ix].textureResource.GetAddressOf()
    //    );
    //} //BindTextures
    //deviceContext->PSSetSamplers(0, 1, &samplerState); //BindSamplerState

    // �׸���.
    RenderBuffers(deviceContext);

    // ������ �ٲٱ�. FrontBuffer <-> BackBuffer.
    swapChain->Present(1, 0);
}


bool Engine::InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer)
{
    gTransform = {
        DirectX::XMMatrixIdentity(),
        //DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(1.f, 1.f,1.f), DirectX::XMMatrixRotationRollPitchYaw(0.f,0.f, 0.f)), DirectX::XMMatrixTranslation(3.0f, 0.f, 10.f)),
        DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.f,1.f,-5.f,0.f), DirectX::XMVectorSet(0.f,1.f,0.f,0.f), DirectX::XMVectorSet(0.f,1.f,0.f,0.f)),
        DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2.0f,width / float(height),0.01f,1000.0f)
    };
    //DirectX::XMMatrixIdentity() : �ʱⰪ 

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

    ////Texture ����
    ////TextureVertexShader ������ 
    //HRESULT tresult = D3DCompileFromFile(
    //    L"TextureMappingVS.hlsl",
    //    NULL,
    //    NULL,
    //    "main", // wchar(���ϵ� ĳ����) �ƴ϶� L �� �ٿ��� ��.
    //    "vs_5_0",
    //    NULL,
    //    NULL,
    //    &TVertexShaderBuffer, // ������ �� �����ڵ带 �޾ƿ� ����.
    //    NULL
    //);
    //if (FAILED(tresult)) { MessageBox(nullptr, L"���� ���̴� ������ ����", L"����", 0); }

    ////TexturePixelShader ������ 
    //tresult = D3DCompileFromFile(
    //    L"TextureMappingPS.hlsl",
    //    NULL,
    //    NULL,
    //    "main",
    //    "ps_5_0",
    //    NULL,
    //    NULL,
    //    &TPixelShaderBuffer,
    //    NULL
    //);
    //if (FAILED(tresult)) { MessageBox(nullptr, L"�ȼ� ���̴� ������ ����", L"����", 0); }

    ////Load Texture
    //Texture texture;
    //texture.filename = L"../res/textures/dog.jpg";

    //if (FAILED(result))
    //{
    //    return false;
    //}
    //textures.push_back(texture);

    ////TextureVertexShader ���� 
    //tresult = device->CreateVertexShader(
    //vertexShaderBuffer->GetBufferPointer(), // ���� ���� �ּ�.
    //    vertexShaderBuffer->GetBufferSize(), // �󸶸�ŭ ���� ��.
    //    nullptr,
    //    & TVertexShader
    //    );
    //    if (FAILED(tresult)) { MessageBox(nullptr, L"���� ���̴� ���� ����", L"����", 0); }

    //

    //    //TexturePixelShader ���� 
    // tresult = device->CreatePixelShader(
    //        pixelShaderBuffer->GetBufferPointer(),
    //        pixelShaderBuffer->GetBufferSize(),
    //        nullptr,
    //        &TPixelShader
    //    );
    // if (FAILED(tresult)) { MessageBox(nullptr, L"�ȼ� ���̴� ���� ����", L"����", 0); }
    //

    ////pixel shader ���÷� ������Ʈ ���� 
    //D3D11_SAMPLER_DESC samplerDesc;
    //ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    //samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.MinLOD = 0;
    //samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //// ���÷� ������Ʈ ����.
    //tresult = device->CreateSamplerState(&samplerDesc, &samplerState);
    //if (FAILED(tresult))
    //{
    //    MessageBox(nullptr, L"���÷� ������Ʈ ���� ����", L"����", 0);
    //    return false;
    //}


    ////Texture Binding 
    //for (int ix = 0; ix < textures.size(); ++ix)
    //{
    //    deviceContext->PSSetShaderResources(
    //        ix,
    //        1,
    //        textures[ix].textureResource.GetAddressOf()
    //    );
    //}


    //���⼭���� Buffer

    //1. �ﰢ�� ����� 
    // ���� ������ �����.
    // ����(Vertex) �迭.
    // �޼� ��ǥ��.
    //Vertex vertices[] =
    //{
    //    Vertex(0.0f, 0.5f, 0.5f),
    //    Vertex(0.5f, -0.5f, 0.5f),
    //    Vertex(-0.5f, -0.5f, 0.5f)
    //};

    //// ������ ����.
    //vertexCount = ARRAYSIZE(vertices);

    //// ���� ���� �����.
    //D3D11_BUFFER_DESC vertexBufferDesc;
    //ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    //vertexBufferDesc.ByteWidth = sizeof(vertices); // �󸶸�ŭ ������.
    //vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    //vertexBufferDesc.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    //vertexBufferDesc.MiscFlags = 0;
    //vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    //// ������ ���.
    //D3D11_SUBRESOURCE_DATA vertexBufferData;
    //ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    //vertexBufferData.pSysMem = vertices;

    //// ���� ���� ����.
    //result = device->CreateBuffer(
    //    &vertexBufferDesc,
    //    &vertexBufferData,
    //    &vertexBuffer
    //);
    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    ////�Է� ���̾ƿ� 
    //// ������ ���� �� ����� (�Է� ���̾ƿ�).
    //D3D11_INPUT_ELEMENT_DESC layout[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    //};

    //// �Է� ���̾ƿ� ����.
    //result = device->CreateInputLayout(
    //    layout,
    //    ARRAYSIZE(layout),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout
    //);
    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0);
    //    return false;
    //}

    //// ��� ����.
    //if (FAILED(result))
    //{
    //    return false;
    //}


    ////2. quad����� 
    //// ���� ������ �����.
    //// ����(Vertex) �迭.
    //// �޼� ��ǥ��.
    //Vertex vertices2[] =
    //{
    //    Vertex(-0.5f, -0.5f, 0.5f),
    //    Vertex(-0.5f, 0.5f, 0.5f),
    //    Vertex(0.5f, 0.5f, 0.5f),
    //    Vertex(-0.5f, -0.5f, 0.5f),
    //    Vertex(0.5f, 0.5f, 0.5f),
    //    Vertex(0.5f, -0.5f, 0.5f)
    //};

    //// ������ ����.
    //vertexCount2 = ARRAYSIZE(vertices2);

    //// ���� ���� �����.
    //D3D11_BUFFER_DESC vertexBufferDesc2;
    //ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
    //vertexBufferDesc2.ByteWidth = sizeof(vertices2); // �󸶸�ŭ ������.
    //vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    //vertexBufferDesc2.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    //vertexBufferDesc2.MiscFlags = 0;
    //vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;

    //// ������ ���.
    //D3D11_SUBRESOURCE_DATA vertexBufferData2;
    //ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));
    //vertexBufferData2.pSysMem = vertices2;

    //// ���� ���� ����.
    //HRESULT result2 = device->CreateBuffer(
    //    &vertexBufferDesc2,
    //    &vertexBufferData2,
    //    &vertexBuffer2
    //);
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    ////�Է� ���̾ƿ� 
    //// ������ ���� �� ����� (�Է� ���̾ƿ�).
    //D3D11_INPUT_ELEMENT_DESC layout2[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    //   
    //};

    //// �Է� ���̾ƿ� ����.
    //result2 = device->CreateInputLayout(
    //    layout2,
    //    ARRAYSIZE(layout2),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout2
    //);
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0);
    //    return false;
    //}

    //// ��� ����.
    //if (FAILED(result2))
    //{
    //    return false;
    //}


    ////3. QuadUV ����� 
    //// ���� ������ �����.
    //// ����(Vertex) �迭.
    //// �޼� ��ǥ��.
    //VertexUV vertices3[] =
    //{
    //    VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
    //    VertexUV(Vector3f(-0.5f, 0.5f, 0.5f), Vector2f(0.0f, 0.0f)),
    //    VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),

    //    VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
    //    VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),
    //    VertexUV(Vector3f(0.5f, -0.5f, 0.5f), Vector2f(1.0f, 1.0f))
    //};

    //// ������ ����.
    //vertexCount3 = ARRAYSIZE(vertices3);

    //// ���� ���� �����.
    //D3D11_BUFFER_DESC vertexBufferDesc3;
    //ZeroMemory(&vertexBufferDesc3, sizeof(vertexBufferDesc3));
    //vertexBufferDesc3.ByteWidth = sizeof(vertices3); // �󸶸�ŭ ������.
    //vertexBufferDesc3.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    //vertexBufferDesc3.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    //vertexBufferDesc3.MiscFlags = 0;
    //vertexBufferDesc3.Usage = D3D11_USAGE_DEFAULT;

    //// ������ ���.
    //D3D11_SUBRESOURCE_DATA vertexBufferData3;
    //ZeroMemory(&vertexBufferData3, sizeof(vertexBufferData3));
    //vertexBufferData3.pSysMem = vertices3;

    //// ���� ���� ����.
    //HRESULT result3 = device->CreateBuffer(
    //    &vertexBufferDesc3,
    //    &vertexBufferData3,
    //    &vertexBuffer3
    //);

    //if (FAILED(result3))
    //{
    //    MessageBox(nullptr, L"���� ���� ���� ����", L"����", 0);
    //    return false;
    //}
    //

    //// ������ ���� �� ����� (�Է� ���̾ƿ�).

    //D3D11_INPUT_ELEMENT_DESC layout3[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    //    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // �տ� position�� float 4���� 12����Ʈ�ϱ� 12.
    //    
    //};

    //// �Է� ���̾ƿ� ����.
    //result3 = device->CreateInputLayout(
    //    layout3,
    //    ARRAYSIZE(layout3),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout3
    //);
    //if (FAILED(result3)) { MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0); }



    //4.ModelUV
    
    // ���ҽ� ��� �߰�.
    ResourceLoader::LoadModel(filename, &vertices, &indices);

    // ������ ����.
    vertexCount = vertices.size();

    // ���� ���� �����.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount; // �󸶸�ŭ ������.
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ������ ���۷� �� ���̴�.
    vertexBufferDesc.CPUAccessFlags = 0; // ������ �ø��� ���� CPU�� GPU ������ �� �ְ� �ұ�? �츮�� ���� ���ؼ� �ڵ��� �� ������ �����ϰ� ���� ��. 0�� �� �����ϰ�.
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // ������ ���.
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    //vertexBufferData.pSysMem = &vertices;  // ����.
    vertexBufferData.pSysMem = vertices.data();

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

    //�Է� ���̾ƿ� 
    // ������ ���� �� ����� (�Է� ���̾ƿ�).
    D3D11_INPUT_ELEMENT_DESC layout[] =
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
    if (FAILED(result))
    {
        MessageBox(nullptr, L"�Է� ���̾ƿ� ���� ����", L"����", 0);
        return false;
    }

    // ��� ����.
    if (FAILED(result))
    {
        return false;
    }


    // ��� ����(transform buffer) �����.
    //D3D11_BUFFER_DESC transformBufferDesc;
    //ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    //transformBufferDesc.ByteWidth = sizeof(Matrix4f);
    //transformBufferDesc.CPUAccessFlags = 0;
    //transformBufferDesc.MiscFlags = 0;
    //transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    //// ���� ���.
    //transformMatrix = Matrix4f::Identity();

    //// ������ ���.
    //D3D11_SUBRESOURCE_DATA transformBufferData;
    //ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    //transformBufferData.pSysMem = &transformMatrix;

    //// ���� ����.
    //result = device->CreateBuffer(
    //    &transformBufferDesc,
    //    &transformBufferData,
    //    &constantBuffer
    //);

     // ��� ����.(�ٸ�����)
    D3D11_BUFFER_DESC transformBufferDesc;
    ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    transformBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    transformBufferDesc.CPUAccessFlags = 0;
    transformBufferDesc.MiscFlags = 0;
    transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // ���� ���.
    transformMatrix.mWorld = XMMatrixTranspose(gTransform.mWorld);
    transformMatrix.mView = XMMatrixTranspose(gTransform.mView);
    transformMatrix.mProjection = XMMatrixTranspose(gTransform.mProjection);

    // ������ ���.
    D3D11_SUBRESOURCE_DATA transformBufferData;
    ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    transformBufferData.pSysMem = &transformMatrix;

    // ���� ����.
    result = device->CreateBuffer(
        &transformBufferDesc,
        &transformBufferData,
        &cBuffer
    );

    if (FAILED(result))
    {
        MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
        return false;
    }

    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    //// ��� ����2.
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    //// ��� ����3.
    //if (FAILED(result3))
    //{
    //    MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    //// ��� ����3.
    //if (FAILED(result4))
    //{
    //    MessageBox(nullptr, L"��� ���� ���� ����", L"����", 0);
    //    return false;
    //}

    //�ε������� 
    this->count = indices.size();
    //void* data = &indices;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * this->count;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // ������ ���.
    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = indices.data();

    // �ε��� ���� ����.
    result = device->CreateBuffer(
        &indexBufferDesc,
        &indexBufferData,
        &buffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"�ε��� ���� ���� ����", L"����", 0);
        return false;
    }


    return true; //������

}

void Engine::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // �׸���. (Render = Bind + Draw)
    // Bind (BindBuffer)
    //unsigned int stride = sizeof(Vertex); // �ѹ��� �� ���� ���� ��. XYZ�ϱ� *3.
    //unsigned int offset = 0;

    ////Triangle �׸���
    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    //deviceContext->IASetInputLayout(inputLayout);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ////Transform Buffer(��ǥ ����)
    ////transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.5f, 0.0f,0.5f));
    //
    //// ��� �����Ͱ� ��� ���� ���ε�. (����.)
    //deviceContext->VSSetConstantBuffers(0, 1, &cBuffer);
    //// ��� ������ ������Ʈ.
    //deviceContext->UpdateSubresource(cBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// Draw (DrawBuffer)
    //deviceContext->Draw(vertexCount, 0); // �̰� DrawCall�̴�.

    //
    ////Quad �׸��� 
    //// �� ��° ���� ���� ������ �Է� ���̾ƿ� ���ε�
    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
    //deviceContext->IASetInputLayout(inputLayout2);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(-0.25f, 0.0f, -0.25f));

    //// ��� ���� ���� (�� ��° ���� ���ۿ� ���� ��� ����)
    //deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    //// ��� ���� ������Ʈ
    //deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// �׸���
    ////deviceContext->Draw(vertexCount2, 0);
    //

    ////3. QuadUV �׸��� 
    //unsigned int stride2 = sizeof(VertexUV); // �ѹ��� �� ���� ���� ��.
    //unsigned int offset2 = 0;

    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer3, &stride2, &offset2);
    //deviceContext->IASetInputLayout(inputLayout3);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ���� �׸� ���� LineList.

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.7f, 0.0f, 0.7f));


    // // ��� ���� ���� (�� ��° ���� ���ۿ� ���� ��� ����)
    //deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    //// ��� ���� ������Ʈ
    //deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// �׸���
    //deviceContext->Draw(vertexCount3, 0);


    //4.modelUV �׸��� (����)
    // Bind
    unsigned int stride = sizeof(VertexUV); // �ѹ��� �� ���� ���� ��.
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ���� �׸� ���� LineList.

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.3f, 0.3f, 0.3f));

    // ��� ������ ������Ʈ.
    deviceContext->UpdateSubresource(cBuffer, 0, NULL, &transformMatrix, 0, 0);

    // ��� �����Ͱ� ��� ���� ���ε�. (����.)
    deviceContext->VSSetConstantBuffers(0, 1, &cBuffer);
    
    //�׸���
    //deviceContext->Draw(vertexCount, 0);
    deviceContext->DrawIndexed(this->count, 0, 0);

    
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