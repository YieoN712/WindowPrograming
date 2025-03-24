#include <windows.h>
#include <tchar.h>

RECT RectLocation(RECT, int, int);

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
	rect.left = 0;
	rect.right = 200;
	rect.top = 0;
	rect.bottom = 30;
	int x = 0, y = 0;

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int i = 2;i < 10;i++) {
			for (int j = 1;j < 11;j++) {
				if (j < 10) {
					wsprintf(temp, L"%d * %d = %d", i, j, i * j);
					SetTextColor(hDC, RGB(0, 0, 0));
					DrawText(hDC, temp, _tcslen(temp), &rect, DT_CENTER | DT_SINGLELINE);
				}
				else {
					wsprintf(temp, L" ");
				}
				y++;
				rect = RectLocation(rect, x, y);
			}

			if (i == 5) {
				x = 0;
			}
			else {
				x++;
				if (i >= 5) {
					y = 10;
				}
				else {
					y = 0;
				}
			}
			rect = RectLocation(rect, x, y);
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
	r.left = x * 200;
	r.right = r.left + 200;
	r.top = y * 30;
	r.bottom = r.top + 30;

	return r;
}