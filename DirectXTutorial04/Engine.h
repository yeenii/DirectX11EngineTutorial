#pragma once
#include "D3DApp.h"
#include "Vertex.h"
#include "Matrix4f.h"
#include <wrl.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class Engine : public D3DApp
{
public:
	//TransformBuffer
	Matrix4f transformMatrix;
	Engine(HINSTANCE hInstance, int width, int height, std::wstring title);

	~Engine();

	bool Initialize() override; // 타이머, 씬 등에 대한 초기화
	int Run() override;


	// Mesh Getter / Setter
	Vector3f& Position() { return position; }
	void SetPosition(float x, float y, float z);
	void SetPosition(Vector3f position);

	Vector3f& Rotation() { return rotation; }
	void SetRotation(float x, float y, float z);
	void SetRotation(Vector3f rotation);

	Vector3f& Scale() { return scale; }
	void SetScale(float x, float y, float z);
	void SetScale(Vector3f scale);

	

protected:
	void Update() override;
	void DrawScene() override;
	
private:
	// 장면 초기화.
	bool InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer); //shader,Buffer의 compile, create, bind 있음 
	void RenderBuffers(ID3D11DeviceContext* deviceContext);//BindBuffer, DrawBuffer,UpdateBuffer
	
	// 변수.
	// 쉐이더도 코드이다. (프로그램.)
	// 쉐이더 생성하는 과정.
	// 1. 컴파일.
	// 2. 생성. (컴파일 완료된 이진 파일 필요 .cso)
	ID3D11VertexShader* vertexShader;	// 컴파일 된 이진코드로 생성한 쉐이더.
	ID3D11PixelShader* pixelShader;		// 컴파일 된 이진코드로 생성한 쉐이더.
	ID3DBlob* vertexShaderBuffer;		// .hlsl 쉐이더 코드 파일로 컴파일 된 이진코드.
	ID3DBlob* pixelShaderBuffer;		// .hlsl 쉐이더 코드 파일로 컴파일 된 이진코드.

	// IA - Input Assembler. (렌더링 파이프라인 중에 젤 첫 단계.)	
	int vertexCount = 0;// 점 개수. // 전달할 때 알려줘야 하기 때문에 갖고 있자.
	int vertexCount2 = 0;// 점 개수. // 전달할 때 알려줘야 하기 때문에 갖고 있자.
	ID3D11Buffer* vertexBuffer; // 정점 버퍼. (Vertex 전달 용도.)
	ID3D11Buffer* vertexBuffer2; // 정점 버퍼. (Vertex 전달 용도.)
	ID3D11InputLayout* inputLayout; // 입력 레이아웃. Vertex에 어떤 정보들(x,y,z,u,v,nx,ny,nz 등)이 담길 지.
	ID3D11InputLayout* inputLayout2; // 입력 레이아웃. Vertex에 어떤 정보들(x,y,z,u,v,nx,ny,nz 등)이 담길 지.
	

	//Transform Buffer
	ID3D11Buffer* constantBuffer; //상수버퍼

	// 트랜스폼 데이터.
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;


};

