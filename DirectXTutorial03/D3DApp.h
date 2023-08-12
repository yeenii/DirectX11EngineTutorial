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
	//멤버 메서드 
	D3DApp(HINSTANCE hInstance, int width, int height, std::wstring title);

	virtual ~D3DApp();

	virtual bool Initialize();

	// 인터페이스
	virtual int Run() = 0; // 완전 가상 함수.

protected:
	//프레임워크 메서드
	virtual void Update() = 0; //순가상 함수. D3DApp 자체는 생성 불가능을 상속해서 해당 함수를 반드시 명시해야 함 
	virtual void DrawScene() = 0;//순가상 함수. D3DApp 자체는 생성 불가능을 상속해서 해당 함수를 반드시 명시해야 함

	bool InitializeDirect3D();//Direct3D를 초기화하는 메서드

	ID3D11Device* device;						// d3d 장치.
	ID3D11DeviceContext* deviceContext;			// d3d 장치문맥.
	IDXGISwapChain* swapChain;					// 스왑체인. 페이지 전환위한 교환 사슬. 
	ID3D11RenderTargetView* renderTargetView;	// 렌더타겟.

	D3D11_VIEWPORT viewport;					// 뷰포트 (화면)

};