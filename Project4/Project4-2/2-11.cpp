#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = _T("Window Class Name");
LPCTSTR lpszWindowName = _T("Shape Rotation & Color");
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

struct Shape {
	COLORREF originalColors;
	int position;
};

int selected = 0;
bool keyPressed = false;
bool leftPressed = false, rightPressed = false;
COLORREF randColor;
Shape shape[4];
int order[4] = { 0, 1, 2, 3 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;

	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
		NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	srand((unsigned)time(NULL));

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

void DrawPentagon(HDC hdc, int cx, int cy, int r, int s) {
	POINT pts[5];
	double theta = s * (3.14159 / 2); // 밑변 수평
	for (int i = 0; i < 5; i++) {
		pts[i].x = cx + (int)(r * cos(2 * 3.14159 * i / 5 + theta));
		pts[i].y = cy + (int)(r * sin(2 * 3.14159 * i / 5 + theta));
	}
	Polygon(hdc, pts, 5);
}

void DrawButterfly(HDC hdc, int cx, int cy, bool n) {
	if (n) {
		POINT top1[3] = { {cx - 40, cy - 40}, {cx + 40, cy - 40}, {cx, cy} };
		POINT bottom1[3] = { {cx - 40, cy + 40}, {cx + 40, cy + 40}, {cx, cy} };

		Polygon(hdc, top1, 3);
		Polygon(hdc, bottom1, 3);
	}
	else {
		POINT top2[3] = { {cx, cy}, {cx - 60, cy - 40}, {cx - 60, cy + 40} };
		POINT bottom2[3] = { {cx, cy}, {cx + 60, cy - 40}, {cx + 60, cy + 40} };

		Polygon(hdc, top2, 3);
		Polygon(hdc, bottom2, 3);
	}
}

void DrawCentralShape(HDC hdc, RECT rect) {
	if (selected == 0) return;
	int cx = (rect.left + rect.right) / 2;
	int cy = (rect.top + rect.bottom) / 2;
	int w = 100, h = 100;

	COLORREF drawColor = keyPressed ? randColor : shape[selected - 1].originalColors;
	HBRUSH brush = CreateSolidBrush(drawColor);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	switch (selected) {
	case 1: Ellipse(hdc, cx - w, cy - h / 2, cx + w, cy + h / 2); break;
	case 2: DrawButterfly(hdc, cx, cy, false); break;
	case 3: DrawPentagon(hdc, cx, cy, 50, 1); break;
	case 4: Pie(hdc, cx - 50, cy - 50, cx + 50, cy + 50, cx + 50, cy, cx, cy - 50); break;
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect = { 250,150,550,450 };

	switch (iMessage) {
	case WM_CREATE:
		shape[0] = { RGB(0, 0, 255), 0 };
		shape[1] = { RGB(0, 255, 0), 1 };
		shape[2] = { RGB(255, 0, 0), 2 };
		shape[3] = { RGB(0, 255, 255), 3 };
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'C': case 'S': case 'P': case 'E':
			selected = (wParam == 'C') ? 1 : (wParam == 'S') ? 2 : (wParam == 'P') ? 3 : 4;
			randColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			keyPressed = true;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_LEFT:
			if (!leftPressed) {
				leftPressed = true;
				SetTimer(hWnd, 1, 150, NULL);
			}
			break;
		case VK_RIGHT:
			if (!rightPressed) {
				rightPressed = true;
				SetTimer(hWnd, 2, 150, NULL);
			}
			break;
		case 'Q': case 'q':
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_KEYUP:
		if (wParam == 'C' || wParam == 'S' || wParam == 'P' || wParam == 'E') {
			keyPressed = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == VK_LEFT) {
			leftPressed = false;
			KillTimer(hWnd, 1);
		}
		if (wParam == VK_RIGHT) {
			rightPressed = false;
			KillTimer(hWnd, 2);
		}
		break;

	case WM_TIMER:
		if (wParam == 1 && leftPressed) {
			int temp = order[0];
			order[0] = order[1];
			order[1] = order[2];
			order[2] = order[3];
			order[3] = temp;
		}
		else if (wParam == 2 && rightPressed) {
			int temp = order[3];
			order[3] = order[2];
			order[2] = order[1];
			order[1] = order[0];
			order[0] = temp;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

		for (int i = 0; i < 4; i++) {
			int shapeIndex = order[i]; // 현재 위치에 있는 도형 인덱스
			int x = 0, y = 0;
			switch (i) {
			case 0: x = 200; y = 300; break;
			case 1: x = 400; y = 100; break;
			case 2: x = 600; y = 300; break;
			case 3: x = 400; y = 500; break;
			}

			HBRUSH brush = CreateSolidBrush(shape[shapeIndex].originalColors);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

			switch (shape[shapeIndex].position) {
			case 0: Ellipse(hdc, x - 50, y - 50, x + 50, y + 50); break;
			case 1: DrawButterfly(hdc, x, y, true); break;
			case 2: DrawPentagon(hdc, x, y, 50, -1); break;
			case 3: Pie(hdc, x - 50, y - 50, x + 50, y + 50, x, y - 50, x + 50, y); break;
			}

			SelectObject(hdc, oldBrush);
			DeleteObject(brush);
		}


		DrawCentralShape(hdc, rect);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
