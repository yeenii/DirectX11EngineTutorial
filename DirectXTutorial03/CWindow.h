//참고 블로그 : https://mingyu0403.tistory.com/218
#pragma once

#include <Windows.h>
#include <string> // STL - Standard Template Library.

class CWindow
{
public:
	static bool InitializeWindow();

	static bool InitializeWindow(
		HINSTANCE hInstance,
		int width,
		int height,
		std::wstring title
	);

	// Getter / Setter.
	static HWND WindowHandle() { return hwnd; }

	static HINSTANCE Instance() { return hInstance; }
	static void SetInstance(HINSTANCE hInstance);

	static int Width() { return width; }
	static void SetWidth(int width);

	static int Height() { return height; }
	static void SetHeight(int height);

	static std::wstring Title() { return title; }
	static void SetTitle(std::wstring title);


private:
	static int width;			// 가로 길이.
	static int height;			// 세로 길이.
	static std::wstring title;	// 윈도우 제목(타이틀). wide string 한글 처리에도 문제없도록.
	static HWND hwnd;			// 윈도우 핸들.
	static HINSTANCE hInstance;	// 윈도우 인스턴스.
};