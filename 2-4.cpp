#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <random>

std::mt19937_64 dre;
std::uniform_int_distribution<int> uidX{ 0,600 };
std::uniform_int_distribution<int> uidY{ 0,500 };
std::uniform_int_distribution<int> rgb{ 0,256 };

RECT RectLocation(RECT, int, int);
COLORREF ColorSet();

static int x = uidX(dre), y = uidY(dre), count = 0;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Windows Program 1";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT	ps;
	HDC	hDC;
	static TCHAR temp[80];
	RECT rect = { 0,0,0,0 };
	COLORREF rgb;

	rgb = ColorSet();

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		rect = RectLocation(rect, x, y);

		SetTextColor(hDC, rgb);
		DrawText(hDC, temp, _tcslen(temp), &rect, DT_WORDBREAK | DT_EDITCONTROL);

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		hDC = GetDC(hWnd);

		if (wParam == VK_BACK) {
			if (count != 0) {
				count--;
			}
		}
		else if (wParam == VK_RETURN) {
			y += 20;
		}

		else if (wParam == VK_ESCAPE) {
			exit(0);
		}
		else {
			temp[count++] = wParam;
		}

		temp[count] = '\0';
		InvalidateRect(hWnd, NULL, true);
		ReleaseDC(hWnd, hDC);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

RECT RectLocation(RECT r, int x, int y) {
	r.left = x;
	r.right = r.left + 100;
	r.top = y;
	r.bottom = r.top + 50;

	return r;
}

COLORREF ColorSet() {
	COLORREF color;

	color = RGB(rgb(dre), rgb(dre), rgb(dre));

	return color;
}