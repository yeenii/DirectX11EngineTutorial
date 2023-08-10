

//참고용 블로그 : https://mingyu0403.tistory.com/218
//콘솔창도 띄워주도록 세팅
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "Engine.h"

#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	// 객체.
	Engine engine = Engine(hInstance, 1280, 800, L"그래픽스 엔진");

	// 초기화.
	if (engine.Initialize() == false)
	{
		MessageBox(nullptr, L"엔진 초기화 실패", L"오류", 0);
		exit(-1);
	}
	
	//  실행.
	engine.Run();
}