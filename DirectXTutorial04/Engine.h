#pragma once
#include "D3DApp.h"
#include "Vertex.h"
class Engine : public D3DApp
{
public:
	Engine(HINSTANCE hInstance, int width, int height, std::wstring title);
	~Engine();

	bool Initialize() override; // Ÿ�̸�, �� � ���� �ʱ�ȭ
	int Run() override;

protected:
	void Update() override;
	void DrawScene() override;
	
private:
	// ��� �ʱ�ȭ.
	bool InitializeScene(ID3D11Device* device, ID3DBlob* vertexShaderBuffer);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

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
	ID3D11Buffer* vertexBuffer; // ���� ����. (Vertex ���� �뵵.)
	ID3D11InputLayout* inputLayout; // �Է� ���̾ƿ�. Vertex�� � ������(x,y,z,u,v,nx,ny,nz ��)�� ��� ��.

	// �� ����.
	int vertexCount = 0; // ������ �� �˷���� �ϱ� ������ ���� ����.

};

