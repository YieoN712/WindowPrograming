#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <random>

std::mt19937_64 dre;
std::uniform_int_distribution<int> uid{ 2,10 };
std::uniform_int_distribution<int> Alpha{ 2,26 };
std::uniform_int_distribution<int> rgb{ 0,256 };

int n = 0, m = 0, k = 0;

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

	int cellWidth = rect.right / n;
	int cellHeight = rect.bottom / m;

	switch (iMessage)
	{
	case WM_CREATE:
		n = uid(dre);
		m = uid(dre);
		k = Alpha(dre);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int row = 0; row < m; ++row) {
			for (int col = 0; col < n; ++col) {
				RECT rect;
				rect.left = col * cellWidth;
				rect.right = rect.left + cellWidth;
				rect.top = row * cellHeight;
				rect.bottom = rect.top + cellHeight;

				// 랜덤 문자열 생성 (a ~ a+k-1)
				TCHAR temp[27];
				for (int i = 0; i < k; ++i) {
					temp[i] = 'a' + i;
				}
				temp[k] = '\0';

				// 랜덤 색상
				COLORREF textColor = ColorSet();
				COLORREF bgColor = ColorSet();

				SetTextColor(hDC, textColor);
				SetBkColor(hDC, bgColor);
				FillRect(hDC, &rect, CreateSolidBrush(bgColor));

				DrawText(hDC, temp, k, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
			}
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

COLORREF ColorSet() {
	COLORREF color;

	color = RGB(rgb(dre), rgb(dre), rgb(dre));

	return color;
}