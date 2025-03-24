#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <random>

std::mt19937_64 dre;
std::uniform_int_distribution<int> X{ 0,800 };
std::uniform_int_distribution<int> Y{ 0,600 };

RECT RectLocation(RECT, int, int);

int len;
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
	int x = 0, y = 0;

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int i = 0; i < 11;i++) {
			if (i == 0) {
				wsprintf(temp, L"%d : (%d, %d)", i, 400, 300);
				rect.left = 400;
				rect.right = rect.left + 100;
				rect.top = 300;
				rect.bottom = rect.top + 30;

				SetTextColor(hDC, RGB(0, 0, 0));
				DrawText(hDC, temp, _tcslen(temp), &rect, DT_CENTER | DT_SINGLELINE);
			}
			x = X(dre) / 10;
			y = Y(dre) / 10;
			len = _tcslen(temp);

			wsprintf(temp, L"%d : (%d, %d)", i, x * 10, y * 10);

			rect = RectLocation(rect, x, y);
			SetTextColor(hDC, RGB(0, 0, 0));
			DrawText(hDC, temp, _tcslen(temp), &rect, DT_CENTER | DT_SINGLELINE);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

RECT RectLocation(RECT r, int x, int y) {
	r.left = x * 10;
	r.right = r.left + 100;
	r.top = y * 10;
	r.bottom = r.top + 30;

	return r;
}