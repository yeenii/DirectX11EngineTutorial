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

	bool Initialize() override; // Ÿ�̸�, �� � ���� �ʱ�ȭ
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
	// ��� �ʱ�ȭ.
	bool InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer); //shader,Buffer�� compile, create, bind ���� 
	void RenderBuffers(ID3D11DeviceContext* deviceContext);//BindBuffer, DrawBuffer,UpdateBuffer
	
	// ����.
	// ���̴��� �ڵ��̴�. (���α׷�.)
	// ���̴� �����ϴ� ����.
	// 1. ������.
	// 2. ����. (������ �Ϸ�� ���� ���� �ʿ� .cso)
	ID3D11VertexShader* vertexShader;	// ������ �� �����ڵ�� ������ ���̴�.
	ID3D11PixelShader* pixelShader;		// ������ �� �����ڵ�� ������ ���̴�.
	ID3DBlob* vertexShaderBuffer;		// .hlsl ���̴� �ڵ� ���Ϸ� ������ �� �����ڵ�.
	ID3DBlob* pixelShaderBuffer;		// .hlsl ���̴� �ڵ� ���Ϸ� ������ �� �����ڵ�.

	// IA - Input Assembler. (������ ���������� �߿� �� ù �ܰ�.)	
	int vertexCount = 0;// �� ����. // ������ �� �˷���� �ϱ� ������ ���� ����.
	int vertexCount2 = 0;// �� ����. // ������ �� �˷���� �ϱ� ������ ���� ����.
	ID3D11Buffer* vertexBuffer; // ���� ����. (Vertex ���� �뵵.)
	ID3D11Buffer* vertexBuffer2; // ���� ����. (Vertex ���� �뵵.)
	ID3D11InputLayout* inputLayout; // �Է� ���̾ƿ�. Vertex�� � ������(x,y,z,u,v,nx,ny,nz ��)�� ��� ��.
	ID3D11InputLayout* inputLayout2; // �Է� ���̾ƿ�. Vertex�� � ������(x,y,z,u,v,nx,ny,nz ��)�� ��� ��.
	

	//Transform Buffer
	ID3D11Buffer* constantBuffer; //�������

	// Ʈ������ ������.
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;


};

