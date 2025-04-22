#include <windows.h>
#include <tchar.h>
#include <cstdlib>
#include <cmath>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int command = 0; // 0: 정지, 1: 오른쪽(h), 2: 아래쪽(v)
int dx = 5, dy = 5;

int x = 20, y = 20; // 원의 현재 위치
int rad = 20;

bool moveRight = true;
bool moveDown = true;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
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

	hWnd = CreateWindow(lpszClass, _T("실습 3-1 게임"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

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
	HDC hDC;
	PAINTSTRUCT ps;
	RECT clientRect;
	TCHAR buf[64];

	switch (iMessage)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 70, NULL);
		break;

	case WM_TIMER:
		GetClientRect(hWnd, &clientRect);

		if (command == 1) {
			if (moveRight) {
				x += dx;
				if (x + rad >= clientRect.right) {
					x = clientRect.right - rad;
					y += dy;
					moveRight = false;
				}
			}
			else {
				x -= dx;
				if (x - rad <= 0) {
					x = rad;
					y += dy;
					moveRight = true;
				}
			}
			if (y + rad >= clientRect.bottom)
				y = rad;
		}
		else if (command == 2) {
			if (moveDown) {
				y += dy;
				if (y + rad >= clientRect.bottom) {
					y = clientRect.bottom - rad;
					x += dx;
					moveDown = false;
				}
			}
			else {
				y -= dy;
				if (y - rad <= 0) {
					y = rad;
					x += dx;
					moveDown = true;
				}
			}
			if (x + rad >= clientRect.right)
				x = rad;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;


	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		wsprintf(buf, _T("속도: dx = %d, dy = %d"), dx, dy);
		TextOut(hDC, 10, 10, buf, lstrlen(buf));
		Ellipse(hDC, x - rad, y - rad, x + rad, y + rad);
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'H': case 'h':
			command = 1;
			break;
		case 'V': case 'v':
			command = 2;
			break;
		case 'P': case 'p':
			command = 0;
			break;
		case VK_OEM_PLUS:
			dx += 10;
			dy += 10;
			break;
		case VK_OEM_MINUS:
			if (dx > 10 && dy > 10) {
				dx -= 10;
				dy -= 10;
			}
			break;
		case 'Q': case 'q':
			KillTimer(hWnd, 1);
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}