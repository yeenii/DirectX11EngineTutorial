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
    //transformMatrix = Matrix4f::Scale(scale) * Matrix4f::Rotation(rotation) * Matrix4f::Translation(position);
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
    deviceContext->PSSetSamplers(0, 1, &samplerState); //픽셀 셰이더 샘플러 스테이트 

    // 그리기.
    RenderBuffers(deviceContext);

    // 프레임 바꾸기. FrontBuffer <-> BackBuffer.
    swapChain->Present(1, 0);
}


bool Engine::InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer)
{
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

    //TexturePixelShader 컴파일 
    //TexturePixelShader 생성 

    //pixel shader 샘플러 스테이트 생성 
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // 샘플러 스테이트 생성.
    result = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(result))
    {
        MessageBox(nullptr, L"샘플러 스테이트 생성 실패", L"오류", 0);
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


    //여기서부터 Buffer

    //1. 삼각형 만들기 
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


    //2. quad만들기 
    // 정점 데이터 만들기.
    // 정점(Vertex) 배열.
    // 왼손 좌표계.
    Vertex vertices2[] =
    {
        Vertex(-0.5f, -0.5f, 0.5f),
        Vertex(-0.5f, 0.5f, 0.5f),
        Vertex(0.5f, 0.5f, 0.5f),
        Vertex(-0.5f, -0.5f, 0.5f),
        Vertex(0.5f, 0.5f, 0.5f),
        Vertex(0.5f, -0.5f, 0.5f)
    };

    // 정점의 개수.
    vertexCount2 = ARRAYSIZE(vertices2);

    // 정점 버퍼 만들기.
    D3D11_BUFFER_DESC vertexBufferDesc2;
    ZeroMemory(&vertexBufferDesc2, sizeof(vertexBufferDesc2));
    vertexBufferDesc2.ByteWidth = sizeof(vertices2); // 얼마만큼 읽을까.
    vertexBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    vertexBufferDesc2.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    vertexBufferDesc2.MiscFlags = 0;
    vertexBufferDesc2.Usage = D3D11_USAGE_DEFAULT;

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA vertexBufferData2;
    ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));
    vertexBufferData2.pSysMem = vertices2;

    // 정점 버퍼 생성.
    HRESULT result2 = device->CreateBuffer(
        &vertexBufferDesc2,
        &vertexBufferData2,
        &vertexBuffer2
    );
    if (FAILED(result2))
    {
        MessageBox(nullptr, L"정점 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    //3. QuadUV 만들기 
    // 정점 데이터 만들기.
    // 정점(Vertex) 배열.
    // 왼손 좌표계.
    VertexUV vertices3[] =
    {
        VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
        VertexUV(Vector3f(-0.5f, 0.5f, 0.5f), Vector2f(0.0f, 0.0f)),
        VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),

        VertexUV(Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 1.0f)),
        VertexUV(Vector3f(0.5f, 0.5f, 0.5f), Vector2f(1.0f, 0.0f)),
        VertexUV(Vector3f(0.5f, -0.5f, 0.5f), Vector2f(1.0f, 1.0f))
    };

    // 정점의 개수.
    vertexCount3 = ARRAYSIZE(vertices3);

    // 정점 버퍼 만들기.
    D3D11_BUFFER_DESC vertexBufferDesc3;
    ZeroMemory(&vertexBufferDesc3, sizeof(vertexBufferDesc3));
    vertexBufferDesc3.ByteWidth = sizeof(vertices3); // 얼마만큼 읽을까.
    vertexBufferDesc3.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 데이터 버퍼로 쓸 것이다.
    vertexBufferDesc3.CPUAccessFlags = 0; // 성능을 올리기 위해 CPU가 GPU 접근할 수 있게 할까? 우리가 구분 잘해서 코딩할 수 있으면 접근하게 만들어도 됨. 0은 못 접근하게.
    vertexBufferDesc3.MiscFlags = 0;
    vertexBufferDesc3.Usage = D3D11_USAGE_DEFAULT;

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA vertexBufferData3;
    ZeroMemory(&vertexBufferData3, sizeof(vertexBufferData3));
    vertexBufferData3.pSysMem = vertices3;

    // 정점 버퍼 생성.
    HRESULT result3 = device->CreateBuffer(
        &vertexBufferDesc3,
        &vertexBufferData3,
        &vertexBuffer3
    );

    if (FAILED(result3))
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
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        
    };

    //QuadUV
    D3D11_INPUT_ELEMENT_DESC layout2[] =
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
    if (FAILED(result)) { MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0); }

    // //QuadUV. 입력 레이아웃 설정.
    result2 = device->CreateInputLayout(
        layout2,
        ARRAYSIZE(layout2),
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &inputLayout3
    );
    if (FAILED(result2)) { MessageBox(nullptr, L"입력 레이아웃 생성 실패", L"오류", 0); }




    // 상수 버퍼(transform buffer) 만들기.
    D3D11_BUFFER_DESC transformBufferDesc;
    ZeroMemory(&transformBufferDesc, sizeof(transformBufferDesc));
    transformBufferDesc.ByteWidth = sizeof(Matrix4f);
    transformBufferDesc.CPUAccessFlags = 0;
    transformBufferDesc.MiscFlags = 0;
    transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    transformBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // 단위 행렬.
    transformMatrix = Matrix4f::Identity();

    // 데이터 담기.
    D3D11_SUBRESOURCE_DATA transformBufferData;
    ZeroMemory(&transformBufferData, sizeof(transformBufferData));
    transformBufferData.pSysMem = &transformMatrix;

    // 버퍼 생성.
    result = device->CreateBuffer(
        &transformBufferDesc,
        &transformBufferData,
        &constantBuffer
    );
    if (FAILED(result))
    {
        MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    // 상수 버퍼2.
    if (FAILED(result2))
    {
        MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    // 상수 버퍼3.
    if (FAILED(result3))
    {
        MessageBox(nullptr, L"상수 버퍼 생성 실패", L"오류", 0);
        return false;
    }

    return true; //마지막

}

void Engine::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // 그리기. (Render = Bind + Draw)
    // Bind (BindBuffer)
    unsigned int stride = sizeof(Vertex); // 한번에 몇 개씩 읽을 지. XYZ니까 *3.
    unsigned int offset = 0;

    //Triangle 그리기
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Transform Buffer(좌표 설정)
    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.5f, 0.0f,0.5f));
    
    // 행렬 데이터가 담긴 버퍼 바인딩. (설정.)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
    // 행렬 데이터 업데이트.
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    // Draw (DrawBuffer)
    deviceContext->Draw(vertexCount, 0); // 이게 DrawCall이다.

    
    //Quad 그리기 
    // 두 번째 정점 버퍼 설정과 입력 레이아웃 바인딩
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
    deviceContext->IASetInputLayout(inputLayout2);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(-0.25f, 0.0f, -0.25f));

    // 상수 버퍼 설정 (두 번째 정점 버퍼에 대한 상수 버퍼)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    // 상수 버퍼 업데이트
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);

    // 그리기
    deviceContext->Draw(vertexCount2, 0);
    

    //3. QuadUV 그리기 
    unsigned int stride2 = sizeof(VertexUV); // 한번에 몇 개씩 읽을 지.
    //unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer3, &stride2, &offset);
    deviceContext->IASetInputLayout(inputLayout3);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 선을 그릴 때는 LineList.

    transformMatrix = Matrix4f::Scale(Vector3f(0.5f, 0.5f, 0.5f)) * Matrix4f::Rotation(Vector3f()) * Matrix4f::Translation(Vector3f(0.7f, 0.0f, 0.7f));


     // 상수 버퍼 설정 (세 번째 정점 버퍼에 대한 상수 버퍼)
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    // 상수 버퍼 업데이트
    deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &transformMatrix, 0, 0);


    // 그리기
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