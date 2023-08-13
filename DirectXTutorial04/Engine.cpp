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
    // 창 만들기.
    // 장치 생성.
    if (D3DApp::Initialize() == false)
    {
        return false;
    }

    // 장면 초기화.
    if (InitializeScene(device, vertexShaderBuffer) == false)
    {
        return false;
    }

    return true;
}

// 1. 메시지 처리 루프.
// 2. 엔진 루프.
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
    // 배경 색상 고르기.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };

    // 지우기 (Clear) - 실제로는 덮어씌워서 색칠하기.
    // Begin Draw(Render) - DX9.
    deviceContext->ClearRenderTargetView(renderTargetView, backgroundColor);

    // 그리기 준비.
    //vertexShader.Bind(deviceContext);//VertexSetShader
    //pixelShader.Bind(deviceContext); //PixelSetShader
    deviceContext->VSSetShader(vertexShader, NULL, NULL);
    deviceContext->PSSetShader(pixelShader, NULL, NULL);

    // 그리기.
    RenderBuffers(deviceContext);


    // 프레임 바꾸기. FrontBuffer <-> BackBuffer.
    swapChain->Present(1, 0);
}


bool Engine::InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer)
{
    // VS 컴파일
    HRESULT result = D3DCompileFromFile(
        L"BasicVS.hlsl",
        NULL,
        NULL,
        "main", // wchar(와일드 캐릭터) 아니라서 L 안 붙여도 됨.
        "vs_5_0",
        NULL,
        NULL,
        &vertexShaderBuffer, // 컴파일 된 이진코드를 받아올 버퍼.
        NULL
    );
    if (FAILED(result)) { MessageBox(nullptr, L"정점 쉐이더 컴파일 실패", L"오류", 0); }

    // VS 생성.
    result = device->CreateVertexShader(
        vertexShaderBuffer->GetBufferPointer(), // 시작 지점 주소.
        vertexShaderBuffer->GetBufferSize(), // 얼마만큼 읽을 지.
        nullptr,
        &vertexShader
    );
    if (FAILED(result)) { MessageBox(nullptr, L"정점 쉐이더 생성 실패", L"오류", 0); }



    // VS랑 하는 방식은 똑같음. 파일명이랑 변수명만 조금씩 바꿔주자.
    // PS 컴파일.
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
    if (FAILED(result)) { MessageBox(nullptr, L"픽셀 쉐이더 컴파일 실패", L"오류", 0); }

    // PS 생성.
    result = device->CreatePixelShader(
        pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        nullptr,
        &pixelShader
    );
    if (FAILED(result)) { MessageBox(nullptr, L"픽셀 쉐이더 생성 실패", L"오류", 0); }


    // 정점 데이터 만들기.
    // 정점(Vertex) 배열.
    // 왼손 좌표계.
    Vertex vertices[] =
    {
        Vertex(0.0f, 0.5f, 0.5f),
        Vertex(0.5f, -0.5f, 0.5f),
        Vertex(-0.5f, -0.5f, 0.5f)
    };

    // 정점의 개수.
    vertexCount = ARRAYSIZE(vertices);

    // 정점 버퍼 만들기.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(vertices); // 얼마만큼 읽을까.
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    vertexBufferDesc.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = vertices;

    // 정점 버퍼 생성.
    result = device->CreateBuffer(
        &vertexBufferDesc,
        &vertexBufferData,
        &vertexBuffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"정점 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    // 정점 데이터 설정하기.

    // 정점에 대한 명세 만들기 (입력 레이아웃).
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

    // 입력 레이아웃 설정.
    result = device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &inputLayout
    );
    if (FAILED(result)) { MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0); }

    // 명세 설정하기.

    return true;
}

void Engine::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // 그리기. (Render = Bind + Draw)
    // Bind
    unsigned int stride = sizeof(Vertex); // 한번에 몇 개씩 읽을 지. XYZ니까 *3.
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 선을 그릴 때는 LineList.

    // Draw
    deviceContext->Draw(vertexCount, 0); // 이게 DrawCall이다.
}

