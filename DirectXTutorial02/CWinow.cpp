#include "CWindow.h"


int CWindow::width;
int CWindow::height;
std::wstring CWindow::title;
HWND CWindow::hwnd;
HINSTANCE CWindow::hInstance;

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	return 0;

	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(
				NULL,
				TEXT("종료하시겠습니까?"),
				TEXT("종료"),
				MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				// 창 삭제.
				DestroyWindow(hwnd);
			}
		}
	}
	return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool CWindow::InitializeWindow()
{
	return InitializeWindow(hInstance, width, height, title);
}

bool CWindow::InitializeWindow(
	HINSTANCE hInstance,
	int width,
	int height,
	std::wstring title)
{
	CWindow::hInstance = hInstance;
	CWindow::width = width;
	CWindow::height = height;
	CWindow::title = title;

	// 윈도우 클래스 선언.
	WNDCLASS wc;

	// 모두 0으로 초기화.
	ZeroMemory(&wc, sizeof(wc));

	// 필요한 값만 설정.
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT("GraphicsEngine");
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;

	// 윈도우 클래스 등록.
	if (RegisterClass(&wc) == false)
	{
		exit(-1);
	}

	// 사각형 조정
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	CWindow::width = rect.right - rect.left;
	CWindow::height = rect.bottom - rect.top;

	// 창 생성 (만들기).
	// 창이 문제 없이 생성되면 핸들 값 반환.
	hwnd = CreateWindow(
		wc.lpszClassName,
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		0, 0,
		CWindow::width, CWindow::height,
		NULL, NULL,
		hInstance,
		NULL
	);

	// 오류 검사.
	if (hwnd == NULL)
	{
		exit(-1);
	}

	// 창 보이기.
	ShowWindow(hwnd, SW_SHOW); // 창 띄우고
	UpdateWindow(hwnd); // 업데이트해야 보임. 한 쌍으로 쓴다고 보면 됨.

	return true;
}

void CWindow::SetInstance(HINSTANCE hInstance)
{
	CWindow::hInstance = hInstance;
}

void CWindow::SetWidth(int width)
{
	CWindow::width = width;
}

void CWindow::SetHeight(int height)
{
	CWindow::height = height;
}

void CWindow::SetTitle(std::wstring title)
{
	CWindow::title = title;
}