#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <random>

std::mt19937_64 dre;
std::uniform_int_distribution<int> uid{ 2,10 };
std::uniform_int_distribution<int> Alpha{ 2,26 };
std::uniform_int_distribution<int> rgb{ 0,256 };

RECT RectLocation(RECT, int, int);
COLORREF ColorSet();

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
	TCHAR temp[100];
	RECT rect;
	rect.left = 10;rect.right = 210;rect.top = 10;rect.bottom = 210;
	int x = 0, y = 0, a = 0;

	switch (iMessage)
	{
	case WM_CREATE:
		x = uid(dre);
		y = uid(dre);
		a = Alpha(dre);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		//for (int i = 0; i < y; i++) {
			//for (int j = 0; j < x; j++) {
				for (int n = 0;n < a;n++) {
					temp[n] = n + 97;
				}
				//rect = RectLocation(rect, j, i);

				SetTextColor(hDC, ColorSet());
				SetBkColor(hDC, ColorSet());
				DrawText(hDC, temp, _tcslen(temp), &rect, DT_WORDBREAK | DT_EDITCONTROL);
			//}
		//}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

RECT RectLocation(RECT r, int x, int y) {
	r.left = x * 100;
	r.right = r.left + 100;
	r.top = y * 300;
	r.bottom = r.top + 300;

	return r;
}

COLORREF ColorSet() {
	COLORREF color;

	color = RGB(rgb(dre), rgb(dre), rgb(dre));

	return color;
}