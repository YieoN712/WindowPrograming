#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <random>

std::mt19937_64 dre;
std::uniform_int_distribution<int> uid{ 0,256 };

RECT RectLocation(RECT, int, int);
COLORREF ColorSet();

static int x = 400, y = 300, c = 0, count = 0;
static COLORREF rgb[10];

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
	static TCHAR temp[10][50];
	static RECT rect[10] = { 0,0,0,0 };

	switch (iMessage)
	{
	case WM_CREATE:
		for (int i = 0;i < 10;i++) {
			rgb[i] = ColorSet();
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int i = 1; i < c;i++) {
			for (int j = 0;j < i;j++) {
				if (j == 0) {
					rect[i - 1] = RectLocation(rect[i - 1], x, y);
					SetTextColor(hDC, rgb[i]);
					DrawText(hDC, temp[i - 1], count, &rect[i - 1], DT_WORDBREAK | DT_EDITCONTROL);
				}
				else {
					rect[i - 1] = RectLocation(rect[i - 1], x - (j * 5), (y - (j * 20)));
					SetTextColor(hDC, rgb[i]);
					DrawText(hDC, temp[i - 1], count, &rect[i - 1], DT_WORDBREAK | DT_EDITCONTROL);
					
					rect[i - 1] = RectLocation(rect[i - 1], x- (j * 5), (y + (j * 20)));
					SetTextColor(hDC, rgb[i]);
					DrawText(hDC, temp[i - 1], count, &rect[i - 1], DT_WORDBREAK | DT_EDITCONTROL);
				}
			}
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		hDC = GetDC(hWnd);

		if (wParam == VK_F1) {
			count = 0;
			c = 0;
		}
		else if (wParam == VK_RETURN) {
			c++;
			count = 0;
		}
		else if (wParam == VK_ESCAPE) {
			exit(0);
		}
		else {
			temp[c][count++] = wParam;
		}

		for (int i = 0; i <= c; i++) {
			for (int j = 0; j < count;j++) {
				if (count == (j + 1)) {
					temp[i][count] = '\0';
				}
			}
		}

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
	r.bottom = r.top + 20;

	return r;
}

COLORREF ColorSet() {
	COLORREF color;

	color = RGB(uid(dre), uid(dre), uid(dre));

	return color;
}