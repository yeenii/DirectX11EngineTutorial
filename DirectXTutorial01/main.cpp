#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include <Windows.h>

// 창의 이벤트를 처리하는 함수.
// 콜백(Callback).
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	// 기본 처리만. 윈도우 디폴드 설정에 따라 처리해달라는 것.
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	// 윈도우 클래스 선언.
	WNDCLASS wc;

	// 모두 0으로 초기화.
	ZeroMemory(&wc, sizeof(wc));

	// 필요한 값만 설정 (밑에서 다시 설명.)
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT("GraphicsEngine");
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;

		// 윈도우 클래스 등록.
		if (RegisterClass(&wc) == false) // 설정 값을 지정할 때, 실수했을 수도 있으니까.
		{
			exit(-1);
		}

	// 창 생성 (만들기).
	// 창이 문제 없이 생성되면 핸들 값 반환.
	HWND hwnd = CreateWindow( // 함수 파라미터를 볼 때는 Ctrl+Shift+Space 누르면 보임.
		wc.lpszClassName, // 등록한 클래스 이름으로 지정을 해줘야한다.
		TEXT("그래픽스 엔진"),
		WS_OVERLAPPEDWINDOW,
		0, 0,
		1280, 800,
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

	

	// 메시지 처리 루프.
// 엔진 루프.
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		// 메시지 처리.
		// GetMessage(&msg, hwnd, 0, 0);
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) // PM_REMOVE의 자리는 이 메세지를 쓰고 어떡할거냐.의 의미인데 지운다는 것임.
		{
			// 메시지 해석해줘.
			TranslateMessage(&msg);
			// 메시지를 처리해야할 곳에 전달해줘.
			DispatchMessage(&msg);
		}
		else
		{
			// 우리의 일.
			// 그리기 (DirectX 11).
		}
	}
}