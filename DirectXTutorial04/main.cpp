//����� ��α� : https://mingyu0403.tistory.com/218
//�ܼ�â�� ����ֵ��� ����
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "Engine.h"
#include "Vector3f.h"

#include <iostream>

#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif
{
	//// ��ü.
	//Engine engine = Engine(hInstance, 1280, 800, L"�׷��Ƚ� ����");

	//// �ʱ�ȭ.
	//if (engine.Initialize() == false)
	//{
	//	MessageBox(nullptr, L"���� �ʱ�ȭ ����", L"����", 0);
	//	exit(-1);
	//}
	//
	////  ����.
	//engine.Run();
	Vector3f vector(1, 1, 1);
	Vector3f vector2(2, 2, 2);
	Vector3f vector3 = vector + vector2;
}