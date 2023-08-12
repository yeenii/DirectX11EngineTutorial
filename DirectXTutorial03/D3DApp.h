#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "CWindow.h"

using namespace DirectX;

class D3DApp
{
public:
	//��� �޼��� 
	D3DApp(HINSTANCE hInstance, int width, int height, std::wstring title);

	virtual ~D3DApp();

	virtual bool Initialize();

	// �������̽�
	virtual int Run() = 0; // ���� ���� �Լ�.

protected:
	//�����ӿ�ũ �޼���
	virtual void Update() = 0; //������ �Լ�. D3DApp ��ü�� ���� �Ұ����� ����ؼ� �ش� �Լ��� �ݵ�� ����ؾ� �� 
	virtual void DrawScene() = 0;//������ �Լ�. D3DApp ��ü�� ���� �Ұ����� ����ؼ� �ش� �Լ��� �ݵ�� ����ؾ� ��

	bool InitializeDirect3D();//Direct3D�� �ʱ�ȭ�ϴ� �޼���

	ID3D11Device* device;						// d3d ��ġ.
	ID3D11DeviceContext* deviceContext;			// d3d ��ġ����.
	IDXGISwapChain* swapChain;					// ����ü��. ������ ��ȯ���� ��ȯ �罽. 
	ID3D11RenderTargetView* renderTargetView;	// ����Ÿ��.

	D3D11_VIEWPORT viewport;					// ����Ʈ (ȭ��)

};