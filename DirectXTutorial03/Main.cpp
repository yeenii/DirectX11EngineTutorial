//참고용 블로그 : https://mingyu0403.tistory.com/218
//콘솔창도 띄워주도록 세팅
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "CWindow.h"

#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	// Window 분리한 것 부르기
	if (CWindow::InitializeWindow(hInstance, 1600, 900, L"인하D3D 엔진") == false)
	{
		exit(-1);
	}

	// 메시지 처리 루프
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		// GetMessage(&msg, hwnd, 0, 0);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 우리의 일.
			// 그리기 (DirectX 11).
		}
	}
}