#include <windows.h>
#include <tchar.h>
#include <cstdlib>
#include <vector>

#define BOARD_SIZE 40
#define CELL_SIZE 20
#define WINDOW_WIDTH (BOARD_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * CELL_SIZE)

enum ShapeType { CIRCLE, RECTANGLE };

struct Shape {
	ShapeType type;
	int x, y;
	int size;
	COLORREF color;
	bool selected;
	bool isTail;
	bool jumping;
};

std::vector<Shape> shapes;
int selectedIndex = -1;
int speed = 10;
std::vector<POINT> obstacles;

void DrawBoard(HDC);
void AddRandomShape(ShapeType);
void DrawShape(HDC, const Shape&);
void ToggleJumping();

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
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

	hWnd = CreateWindow(lpszClass, _T("실습 3-2 게임"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (WINDOW_WIDTH + CELL_SIZE - 1), (WINDOW_HEIGHT + BOARD_SIZE - 1), NULL, NULL, hInstance, NULL);

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
	PAINTSTRUCT ps;
	HDC hDC;

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		DrawBoard(hDC);

		for (int i = 0; i < shapes.size(); ++i) {
			DrawShape(hDC, shapes[i]);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'e': case 'E':
			AddRandomShape(CIRCLE);
			break;

		case 'r': case 'R':
			AddRandomShape(RECTANGLE);
			break;

		case 'p': case 'P':
			shapes.clear();
			selectedIndex = -1;
			break;

		case 'q': case 'Q':
			PostQuitMessage(0);
			break;

		case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
			if (selectedIndex != -1) {
				Shape& s = shapes[selectedIndex];

				if (wParam == VK_LEFT) {
					s.x -= speed;
					if (s.x < 0) s.x = BOARD_SIZE - 1;
				}

				if (wParam == VK_RIGHT) {
					s.x += speed;
					if (s.x >= BOARD_SIZE) s.x = 0;
				}

				if (wParam == VK_UP) {
					s.y -= speed;
					if (s.y < 0)s.y = BOARD_SIZE - 1;
				}

				if (wParam == VK_DOWN) {
					s.y += speed;
					if (s.y >= BOARD_SIZE)s.y = 0;
				}

				InvalidateRect(hWnd, NULL, TRUE);
			}

			break;

		case VK_OEM_PLUS:
			speed += 1;
			break;

		case VK_OEM_MINUS:
			if (speed > 1)speed -= 1;
			break;

		case 'j': case 'J':
			ToggleJumping();
			break;

		case 't': case 'T':
			if (selectedIndex != -1) {
				Shape& st = shapes[selectedIndex];

				if (st.size == CELL_SIZE) {
					st.size *= 2;
				}
				else {
					st.size = CELL_SIZE;
				}
			}

			break;

		case 's': case 'S':
			break;

		case 'a': case 'A':
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		int mx = LOWORD(lParam);
		int my = HIWORD(lParam);

		if (selectedIndex != -1) {
			Shape& sl = shapes[selectedIndex];
			sl.x = mx / CELL_SIZE;
			sl.y = my / CELL_SIZE;

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}

	case WM_RBUTTONDOWN:
	{
		if (obstacles.size() < 20) {
			int mx = LOWORD(lParam);
			int my = HIWORD(lParam);

			obstacles.push_back({ mx / CELL_SIZE, my / CELL_SIZE });

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawBoard(HDC hdc) {
	HPEN gridPen = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
	HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

	for (int i = 0; i <= BOARD_SIZE; i++) {
		MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
		LineTo(hdc, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);

		MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
		LineTo(hdc, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(gridPen);
}

void AddRandomShape(ShapeType type) {
	Shape s;
	s.type = type;
	s.x = rand() % BOARD_SIZE;
	s.y = rand() % BOARD_SIZE;
	s.size = CELL_SIZE;
	s.color = RGB(rand() % 256, rand() % 256, rand() % 256);
	s.selected = false;
	s.isTail = false;
	s.jumping = false;

	shapes.push_back(s);
	selectedIndex = shapes.size() - 1;
}

void DrawShape(HDC hdc, const Shape& s) {
	HBRUSH brush = CreateSolidBrush(s.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	HPEN pen = (s.selected) ? CreatePen(PS_SOLID, 2, RGB(2555, 0, 0)) : (HPEN)GetStockObject(NULL_PEN);
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	int px = s.x * CELL_SIZE;
	int py = s.y * CELL_SIZE;
	int sz = s.size;

	switch (s.type)
	{
	case CIRCLE:
		Ellipse(hdc, px, py, px + sz, py + sz);
		break;
	case RECTANGLE:
		Rectangle(hdc, px, py, px + sz, py + sz);
		break;
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	SelectObject(hdc, oldPen);
	if (s.selected)
		DeleteObject(pen);
}

void ToggleJumping() {
	if (selectedIndex != -1) {
		Shape& s = shapes[selectedIndex];
		s.jumping = !s.jumping;  // 점프 상태 토글
	}
}