#pragma once

#include <Windows.h>

#include <Keyboard.h>
#include <Mouse.h>

#include "Vector2f.h"

using DirectX::Keyboard;
using DirectX::Mouse;

class InputProcessor
{
public:
	// ��ġ �ʱ�ȭ.
	static void InitializeInput(HWND hwnd);
	static bool IsKeyDown(Keyboard::Keys key);
	static bool IsKeyUp(Keyboard::Keys key);

	// ����.
	static void ResetKeyboardState() { keyboard.Reset(); }

	// ���콺 �Է�.
	static Vector2f MouseDragState();
	static Mouse::State MouseState();

private:
	// Ű����, ���콺.
	static Keyboard keyboard;
	static Mouse mouse;
};

