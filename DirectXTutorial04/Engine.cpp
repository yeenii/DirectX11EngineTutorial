#include "Engine.h"
#include <d3dcompiler.h>

Engine::Engine(HINSTANCE hInstance, int width, int height, std::wstring title)
    : D3DApp(hInstance, width, height, title)
{

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
    if (InitializeScene() == false)
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
}

void Engine::DrawScene()
{
    // ���� ������.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
    
    // ����� (Clear) - �����δ� ������� ��ĥ�ϱ�.
    // Begin Draw(Render) - DX9.
    deviceContext->ClearRenderTargetView(renderTargetView, backgroundColor);

    // ������ �ٲٱ�. FrontBuffer <-> BackBuffer.
    swapChain->Present(1, 0);
}

bool Engine::InitializeScene()
{
    // VS ������
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

    // VS�� �ϴ� ����� �Ȱ���. ���ϸ��̶� �������� ���ݾ� �ٲ�����.
    // PS ������.
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

    // ���� ������ �����.

    // ���� ������ �����ϱ�.

    // ������ ���� ���� ����� (�Է� ���̾ƿ�).
    //LPCSTR SemanticName;
    //UINT SemanticIndex;
    //DXGI_FORMAT Format;
    //UINT InputSlot;
    //UINT AlignedByteOffset;
    //D3D11_INPUT_CLASSIFICATION InputSlotClass;
    //UINT InstanceDataStepRate;
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

    // ���� �����ϱ�.

    return true;
}