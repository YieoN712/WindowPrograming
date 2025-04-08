#include <windows.h>
#include <tchar.h>

void DrawRects(HDC);
void DrawLines(HDC);
void DrawTriangles(HDC);

int drawMode = 0;

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
	RECT centerRect = { 200,100,550,450 };
	HBRUSH hBrush, oldBrush;

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_CHAR:
		switch (wParam) {
		case 'l':
			drawMode = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 't':
			drawMode = 2;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 'r':
			drawMode = 3;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		FrameRect(hDC, &centerRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

		switch (drawMode) {
		case 1: DrawLines(hDC); break;
		case 2: DrawTriangles(hDC); break;
		case 3: DrawRects(hDC); break;
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawRects(HDC hdc) {
	RECT r = { 210, 90, 230, 110 };
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	for (int i = 0; i < 10; i++) {
		Rectangle(hdc, 190, r.top + i * 30, 210, r.bottom + i * 30);
	}
	for (int i = 0; i < 10; i++) {
		Rectangle(hdc, r.left + i * 30, r.top, r.right + i * 30, r.bottom);
	}
	for (int i = 0; i < 10; i++) {
		Rectangle(hdc, r.left + 330, r.top + i * 30, r.right + 330, r.bottom + i * 30);
	}
	for (int i = 0; i < 10; i++) {
		Rectangle(hdc, r.left + i * 30, r.top + 350, r.right + i * 30, r.bottom + 350);
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);
}

void DrawLines(HDC hdc) {
	HPEN hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
	HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

	for (int i = 0; i < 10; i++) {
		MoveToEx(hdc, 210 + i * 30, 90, NULL);
		LineTo(hdc, 220 + i * 30, 110);
	}
	for (int i = 0; i < 10; i++) {
		MoveToEx(hdc, 210 + i * 30, 440, NULL);
		LineTo(hdc, 220 + i * 30, 460);
	}
	for (int i = 0; i < 10; i++) {
		MoveToEx(hdc, 190, 100 + i * 30, NULL);
		LineTo(hdc, 210, 110 + i * 30);
	}
	for (int i = 0; i < 10; i++) {
		MoveToEx(hdc, 540, 100 + i * 30, NULL);
		LineTo(hdc, 560, 110 + i * 30);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(hPen);
}

void DrawTriangles(HDC hdc) {
	POINT pt[3];
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	int x, y;

	for (int i = 0; i < 40; i++) {
		if (i < 10) {
			x = 240 + i * 30;
			pt[0] = { x, 430 };
			pt[1] = { x + 10, 450 };
			pt[2] = { x + 20, 430 };
			Polygon(hdc, pt, 3);
		}
		else if (i >= 10 and i < 20) {
			y = 120 + (i % 10) * 30;
			pt[0] = { 190, y };
			pt[1] = { 200, y + 20 };
			pt[2] = { 210, y };
			Polygon(hdc, pt, 3);
		}
		else if (i >= 20 and i < 30) {
			y = 120 + (i % 10) * 30;
			pt[0] = { 540, y };
			pt[1] = { 550, y + 20 };
			pt[2] = { 560, y };
			Polygon(hdc, pt, 3);
		}
		else if (i >= 30 and i < 40) {
			x = 240 + (i % 10) * 30;
			pt[0] = { x, 110 };
			pt[1] = { x + 10, 90 };
			pt[2] = { x + 20, 110 };
			Polygon(hdc, pt, 3);
		}
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);
}