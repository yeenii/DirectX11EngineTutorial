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
            //// ESC 종료.
            //if (InputProcessor::IsKeyDown(Keyboard::Keys::Escape) == true)
            //{
            //    if (MessageBox(nullptr, L"종료하시겠습니까?", L"종료", MB_YESNO) == IDYES)
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
    //업데이트할 때 마다 변경할 것 넣어주기 
    static float t = 0;
    //t += 0.2;
    //transformMatrix = Matrix4f::Scale(scale) * Matrix4f::Rotation(rotation) * Matrix4f::Translation(position);
    transformMatrix.mWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(1.f, 1.f, 1.f), DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f)), DirectX::XMMatrixTranslation(t, t, t)));
    
    
    //// 테스트 용.
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
    // 배경 색상 고르기.
    float backgroundColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };

    // 지우기 (Clear) - 실제로는 덮어씌워서 색칠하기.
    // Begin Draw(Render) - DX9.
    deviceContext->ClearRenderTargetView(renderTargetView, backgroundColor);

    // 그리기 준비.
    deviceContext->VSSetShader(vertexShader, NULL, NULL);
    deviceContext->PSSetShader(pixelShader, NULL, NULL);

    //deviceContext->VSSetShader(TVertexShader, NULL, NULL);
    //deviceContext->PSSetShader(TPixelShader, NULL, NULL);
    //deviceContext->PSSetSamplers(0, 1, &samplerState); //픽셀 셰이더 샘플러 스테이트 

    //그리기 준비 
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

    // 그리기.
    RenderBuffers(deviceContext);

    // 프레임 바꾸기. FrontBuffer <-> BackBuffer.
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
    //DirectX::XMMatrixIdentity() : 초기값 

    // VertexShader 컴파일
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
    // Pixel Shader 컴파일.
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

    ////Texture 매핑
    ////TextureVertexShader 컴파일 
    //HRESULT tresult = D3DCompileFromFile(
    //    L"TextureMappingVS.hlsl",
    //    NULL,
    //    NULL,
    //    "main", // wchar(와일드 캐릭터) 아니라서 L 안 붙여도 됨.
    //    "vs_5_0",
    //    NULL,
    //    NULL,
    //    &TVertexShaderBuffer, // 컴파일 된 이진코드를 받아올 버퍼.
    //    NULL
    //);
    //if (FAILED(tresult)) { MessageBox(nullptr, L"정점 쉐이더 컴파일 실패", L"오류", 0); }

    ////TexturePixelShader 컴파일 
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
    //if (FAILED(tresult)) { MessageBox(nullptr, L"픽셀 쉐이더 컴파일 실패", L"오류", 0); }

    ////Load Texture
    //Texture texture;
    //texture.filename = L"../res/textures/dog.jpg";

    //if (FAILED(result))
    //{
    //    return false;
    //}
    //textures.push_back(texture);

    ////TextureVertexShader 생성 
    //tresult = device->CreateVertexShader(
    //vertexShaderBuffer->GetBufferPointer(), // 시작 지점 주소.
    //    vertexShaderBuffer->GetBufferSize(), // 얼마만큼 읽을 지.
    //    nullptr,
    //    & TVertexShader
    //    );
    //    if (FAILED(tresult)) { MessageBox(nullptr, L"정점 쉐이더 생성 실패", L"오류", 0); }

    //

    //    //TexturePixelShader 생성 
    // tresult = device->CreatePixelShader(
    //        pixelShaderBuffer->GetBufferPointer(),
    //        pixelShaderBuffer->GetBufferSize(),
    //        nullptr,
    //        &TPixelShader
    //    );
    // if (FAILED(tresult)) { MessageBox(nullptr, L"픽셀 쉐이더 생성 실패", L"오류", 0); }
    //

    ////pixel shader 샘플러 스테이트 생성 
    //D3D11_SAMPLER_DESC samplerDesc;
    //ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    //samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //samplerDesc.MinLOD = 0;
    //samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //// 샘플러 스테이트 생성.
    //tresult = device->CreateSamplerState(&samplerDesc, &samplerState);
    //if (FAILED(tresult))
    //{
    //    MessageBox(nullptr, L"샘플러 스테이트 생성 실패", L"오류", 0);
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


    //여기서부터 Buffer

    //1. 삼각형 만들기 
    // 정점 데이터 만들기.
    // 정점(Vertex) 배열.
    // 왼손 좌표계.
    //Vertex vertices[] =
    //{
    //    Vertex(0.0f, 0.5f, 0.5f),
    //    Vertex(0.5f, -0.5f, 0.5f),
    //    Vertex(-0.5f, -0.5f, 0.5f)
    //};

    //// 정점의 개수.
    //vertexCount = ARRAYSIZE(vertices);

    //// 정점 버퍼 만들기.
    //D3D11_BUFFER_DESC vertexBufferDesc;
    //ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    //vertexBufferDesc.ByteWidth = sizeof(vertices); // 얼마만큼 읽을까.
    //vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    //vertexBufferDesc.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    //vertexBufferDesc.MiscFlags = 0;
    //vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    //// 데이터 담기.
    //D3D11_SUBRESOURCE_DATA vertexBufferData;
    //ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    //vertexBufferData.pSysMem = vertices;

    //// 정점 버퍼 생성.
    //result = device->CreateBuffer(
    //    &vertexBufferDesc,
    //    &vertexBufferData,
    //    &vertexBuffer
    //);
    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"정점 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    ////입력 레이아웃 
    //// 정점에 대한 명세 만들기 (입력 레이아웃).
    //D3D11_INPUT_ELEMENT_DESC layout[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    //};

    //// 입력 레이아웃 설정.
    //result = device->CreateInputLayout(
    //    layout,
    //    ARRAYSIZE(layout),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout
    //);
    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0);
    //    return false;
    //}

    //// 상수 버퍼.
    //if (FAILED(result))
    //{
    //    return false;
    //}


    ////2. quad만들기 
    //// 정점 데이터 만들기.
    //// 정점(Vertex) 배열.
    //// 왼손 좌표계.
    //Vertex vertices2[] =
    //{
    //    Vertex(-0.5f, -0.5f, 0.5f),
    //    Vertex(-0.5f, 0.5f, 0.5f),
    //    Vertex(0.5f, 0.5f, 0.5f),
    //    Vertex(-0.5f, -0.5f, 0.5f),
    //    Vertex(0.5f, 0.5f, 0.5f),
    //    Vertex(0.5f, -0.5f, 0.5f)
    //};

    //// 정점의 개수.
    //vertexCount2 = ARRAYSIZE(vertices2);

    //// 정점 버퍼 만들기.
    //D3D11_BUFFER_DESC vertexBufferDesc2;
    //ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
    //vertexBufferDesc2.ByteWidth = sizeof(vertices2); // 얼마만큼 읽을까.
    //vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    //vertexBufferDesc2.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    //vertexBufferDesc2.MiscFlags = 0;
    //vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;

    //// 데이터 담기.
    //D3D11_SUBRESOURCE_DATA vertexBufferData2;
    //ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));
    //vertexBufferData2.pSysMem = vertices2;

    //// 정점 버퍼 생성.
    //HRESULT result2 = device->CreateBuffer(
    //    &vertexBufferDesc2,
    //    &vertexBufferData2,
    //    &vertexBuffer2
    //);
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"정점 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    ////입력 레이아웃 
    //// 정점에 대한 명세 만들기 (입력 레이아웃).
    //D3D11_INPUT_ELEMENT_DESC layout2[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    //   
    //};

    //// 입력 레이아웃 설정.
    //result2 = device->CreateInputLayout(
    //    layout2,
    //    ARRAYSIZE(layout2),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout2
    //);
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0);
    //    return false;
    //}

    //// 상수 버퍼.
    //if (FAILED(result2))
    //{
    //    return false;
    //}


    ////3. QuadUV 만들기 
    //// 정점 데이터 만들기.
    //// 정점(Vertex) 배열.
    //// 왼손 좌표계.
    //VertexUV vertices3[] =
    //{
    //    VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
    //    VertexUV(Vector3f(-0.5f, 0.5f, 0.5f), Vector2f(0.0f, 0.0f)),
    //    VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),

    //    VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
    //    VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),
    //    VertexUV(Vector3f(0.5f, -0.5f, 0.5f), Vector2f(1.0f, 1.0f))
    //};

    //// 정점의 개수.
    //vertexCount3 = ARRAYSIZE(vertices3);

    //// 정점 버퍼 만들기.
    //D3D11_BUFFER_DESC vertexBufferDesc3;
    //ZeroMemory(&vertexBufferDesc3, sizeof(vertexBufferDesc3));
    //vertexBufferDesc3.ByteWidth = sizeof(vertices3); // 얼마만큼 읽을까.
    //vertexBufferDesc3.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    //vertexBufferDesc3.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    //vertexBufferDesc3.MiscFlags = 0;
    //vertexBufferDesc3.Usage = D3D11_USAGE_DEFAULT;

    //// 데이터 담기.
    //D3D11_SUBRESOURCE_DATA vertexBufferData3;
    //ZeroMemory(&vertexBufferData3, sizeof(vertexBufferData3));
    //vertexBufferData3.pSysMem = vertices3;

    //// 정점 버퍼 생성.
    //HRESULT result3 = device->CreateBuffer(
    //    &vertexBufferDesc3,
    //    &vertexBufferData3,
    //    &vertexBuffer3
    //);

    //if (FAILED(result3))
    //{
    //    MessageBox(nullptr, L"정점 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}
    //

    //// 정점에 대한 명세 만들기 (입력 레이아웃).

    //D3D11_INPUT_ELEMENT_DESC layout3[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    //    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // 앞에 position이 float 4개라서 12바이트니까 12.
    //    
    //};

    //// 입력 레이아웃 설정.
    //result3 = device->CreateInputLayout(
    //    layout3,
    //    ARRAYSIZE(layout3),
    //    vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(),
    //    &inputLayout3
    //);
    //if (FAILED(result3)) { MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0); }



    //4.ModelUV
    
    // 리소스 경로 추가.
    ResourceLoader::LoadModel(filename, &vertices, &indices);

    // 정점의 개수.
    vertexCount = vertices.size();

    // 정점 버퍼 만들기.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount; // 얼마만큼 읽을까.
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    vertexBufferDesc.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    //vertexBufferData.pSysMem = &vertices;  // 같음.
    vertexBufferData.pSysMem = vertices.data();

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

    //입력 레이아웃 
    // 정점에 대한 명세 만들기 (입력 레이아웃).
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // 앞에 position이 float 4개라서 12바이트니까 12.
    };

    // 입력 레이아웃 설정.
    result = device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &inputLayout
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0);
        return false;
    }

    // 상수 버퍼.
    if (FAILED(result))
    {
        return false;
    }


    // 상수 버퍼(transform buffer) 만들기.
    //D3D11_BUFFER_DESC transformBufferDesc;
    //ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    //transformBufferDesc.ByteWidth = sizeof(Matrix4f);
    //transformBufferDesc.CPUAccessFlags = 0;
    //transformBufferDesc.MiscFlags = 0;
    //transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    //// 단위 행렬.
    //transformMatrix = Matrix4f::Identity();

    //// 데이터 담기.
    //D3D11_SUBRESOURCE_DATA transformBufferData;
    //ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    //transformBufferData.pSysMem = &transformMatrix;

    //// 버퍼 생성.
    //result = device->CreateBuffer(
    //    &transformBufferDesc,
    //    &transformBufferData,
    //    &constantBuffer
    //);

     // 상수 버퍼.(다른버전)
    D3D11_BUFFER_DESC transformBufferDesc;
    ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    transformBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    transformBufferDesc.CPUAccessFlags = 0;
    transformBufferDesc.MiscFlags = 0;
    transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // 단위 행렬.
    transformMatrix.mWorld = XMMatrixTranspose(gTransform.mWorld);
    transformMatrix.mView = XMMatrixTranspose(gTransform.mView);
    transformMatrix.mProjection = XMMatrixTranspose(gTransform.mProjection);

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA transformBufferData;
    ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    transformBufferData.pSysMem = &transformMatrix;

    // 버퍼 생성.
    result = device->CreateBuffer(
        &transformBufferDesc,
        &transformBufferData,
        &cBuffer
    );

    if (FAILED(result))
    {
        MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    //if (FAILED(result))
    //{
    //    MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    //// 상수 버퍼2.
    //if (FAILED(result2))
    //{
    //    MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    //// 상수 버퍼3.
    //if (FAILED(result3))
    //{
    //    MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    //// 상수 버퍼3.
    //if (FAILED(result4))
    //{
    //    MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
    //    return false;
    //}

    //인덱스버퍼 
    this->count = indices.size();
    //void* data = &indices;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * this->count;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = indices.data();

    // 인덱스 버퍼 생성.
    result = device->CreateBuffer(
        &indexBufferDesc,
        &indexBufferData,
        &buffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"인덱스 버퍼 생성 실패", L"오류", 0);
        return false;
    }


    return true; //마지막

}

void Engine::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // 그리기. (Render = Bind + Draw)
    // Bind (BindBuffer)
    //unsigned int stride = sizeof(Vertex); // 한번에 몇 개씩 읽을 지. XYZ니까 *3.
    //unsigned int offset = 0;

    ////Triangle 그리기
    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    //deviceContext->IASetInputLayout(inputLayout);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ////Transform Buffer(좌표 설정)
    ////transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.5f, 0.0f,0.5f));
    //
    //// 행렬 데이터가 담긴 버퍼 바인딩. (설정.)
    //deviceContext->VSSetConstantBuffers(0, 1, &cBuffer);
    //// 행렬 데이터 업데이트.
    //deviceContext->UpdateSubresource(cBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// Draw (DrawBuffer)
    //deviceContext->Draw(vertexCount, 0); // 이게 DrawCall이다.

    //
    ////Quad 그리기 
    //// 두 번째 정점 버퍼 설정과 입력 레이아웃 바인딩
    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
    //deviceContext->IASetInputLayout(inputLayout2);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(-0.25f, 0.0f, -0.25f));

    //// 상수 버퍼 설정 (두 번째 정점 버퍼에 대한 상수 버퍼)
    //deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    //// 상수 버퍼 업데이트
    //deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// 그리기
    ////deviceContext->Draw(vertexCount2, 0);
    //

    ////3. QuadUV 그리기 
    //unsigned int stride2 = sizeof(VertexUV); // 한번에 몇 개씩 읽을 지.
    //unsigned int offset2 = 0;

    //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer3, &stride2, &offset2);
    //deviceContext->IASetInputLayout(inputLayout3);
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 선을 그릴 때는 LineList.

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.7f, 0.0f, 0.7f));


    // // 상수 버퍼 설정 (세 번째 정점 버퍼에 대한 상수 버퍼)
    //deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    //// 상수 버퍼 업데이트
    //deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    //// 그리기
    //deviceContext->Draw(vertexCount3, 0);


    //4.modelUV 그리기 (보류)
    // Bind
    unsigned int stride = sizeof(VertexUV); // 한번에 몇 개씩 읽을 지.
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 선을 그릴 때는 LineList.

    //transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.3f, 0.3f, 0.3f));

    // 행렬 데이터 업데이트.
    deviceContext->UpdateSubresource(cBuffer, 0, NULL, &transformMatrix, 0, 0);

    // 행렬 데이터가 담긴 버퍼 바인딩. (설정.)
    deviceContext->VSSetConstantBuffers(0, 1, &cBuffer);
    
    //그리기
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