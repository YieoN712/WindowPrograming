#include <windows.h>
#include <tchar.h>
#include <vector>
#include <ctime>
#include <cstdlib>

enum ShapeType { CIRCLE, TRIANGLE, RECTANGLE, STAR };

struct Shape {
	ShapeType type;
	int x, y;
	int size;
	COLORREF color;
	bool selected;

	ShapeType originalType;
	COLORREF originalColor;

	bool transformed = false;
};

std::vector<Shape> shapes;
int selectedIndex = -1;

void DrawShape(HDC, const Shape&);
void AddRandomShape(ShapeType);
void DrawBoard(HDC);

#define BOARD_SIZE 40
#define CELL_SIZE 20
#define WINDOW_WIDTH (BOARD_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * CELL_SIZE)

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

	hWnd = CreateWindow(lpszClass, _T("실습 2-14 게임"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (WINDOW_WIDTH + CELL_SIZE - 1), (WINDOW_HEIGHT + BOARD_SIZE - 1), NULL, NULL, hInstance, NULL);

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

	switch (iMessage)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		DrawBoard(hDC);

		for (int i = 0; i < shapes.size();++i) {
			if (i != selectedIndex) {
				DrawShape(hDC, shapes[i]);
			}
		}
		if (selectedIndex != -1) {
			DrawShape(hDC, shapes[selectedIndex]);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'E': AddRandomShape(CIRCLE); break;
		case 'R': AddRandomShape(RECTANGLE); break;
		case 'T': AddRandomShape(TRIANGLE); break;
		case 'P': shapes.clear(); selectedIndex = -1; break;
		case 'D':
			if (selectedIndex != -1) {
				shapes.erase(shapes.begin() + selectedIndex);
				if (!shapes.empty()) {
					if (selectedIndex >= (int)shapes.size()) {
						selectedIndex = (int)shapes.size() - 1;
					}
					shapes[selectedIndex].selected = true;
				}
				else {
					selectedIndex = -1;
				}
			}
			break;
		case 'Q': PostQuitMessage(0); break;
			// 이동
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			if (selectedIndex != -1) {
				Shape& s = shapes[selectedIndex];
				if (wParam == VK_LEFT) {
					if (s.x > 0)
						s.x--;
					else
						s.x = BOARD_SIZE - 1;
				}
				if (wParam == VK_RIGHT) {
					if (s.x < BOARD_SIZE - 1)
						s.x++;
					else
						s.x = 0;
				}
				if (wParam == VK_UP) {
					if (s.y > 0)
						s.y--;
					else
						s.y = BOARD_SIZE - 1;
				}
				if (wParam == VK_DOWN) {
					if (s.y < BOARD_SIZE - 1)
						s.y++;
					else
						s.y = 0;
				}
			}
			break;
			// 확대/축소
		case VK_OEM_PLUS:
			if (selectedIndex != -1)
				shapes[selectedIndex].size += 5;
			break;
		case VK_OEM_MINUS:
			if (selectedIndex != -1 && shapes[selectedIndex].size > 5)
				shapes[selectedIndex].size -= 5;
			break;
			// 색상/모양 변경
		case 'C':
			if (selectedIndex != -1) {
				ShapeType targetType = shapes[selectedIndex].type;
				bool applyTransform = !shapes[selectedIndex].transformed;
				
				COLORREF c = RGB(rand() % 256, rand() % 256, rand() % 256);

				for (auto& s : shapes) {
					if (s.type == targetType || s.transformed) {
						if (applyTransform && !s.transformed) {
							s.originalType = s.type;
							s.originalColor = s.color;
							s.type = STAR;
							s.color = c;
							s.transformed = true;
						}
						else if (!applyTransform && s.transformed) {
							s.type = s.originalType;
							s.color = s.originalColor;
							s.transformed = false;
						}
					}
				}
			}
			break;
		}

		// 숫자 키로 선택
		if (wParam >= '1' && wParam <= '9') {
			int idx = wParam - '1';
			if (idx < (int)shapes.size()) {
				if (selectedIndex != -1)
					shapes[selectedIndex].selected = false;
				selectedIndex = idx;
				shapes[selectedIndex].selected = true;
			}
		}
		else if (wParam == '0') {
			int idx = 9;
			if (idx < (int)shapes.size()) {
				if (selectedIndex != -1)
					shapes[selectedIndex].selected = false;
				selectedIndex = idx;
				shapes[selectedIndex].selected = true;
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

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
		// 세로
		MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
		LineTo(hdc, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);

		// 가로
		MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
		LineTo(hdc, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(gridPen);
}

void AddRandomShape(ShapeType type) {
	if (shapes.size() >= 10) {
		shapes.erase(shapes.begin()); // 첫 번째 삭제
	}

	Shape s;
	s.type = type;
	s.x = rand() % BOARD_SIZE;
	s.y = rand() % BOARD_SIZE;
	s.size = CELL_SIZE;
	s.color = RGB(rand() % 256, rand() % 256, rand() % 256);
	s.selected = false;
	s.originalType = s.type;
	s.originalColor = s.color;

	shapes.push_back(s);
	selectedIndex = (int)shapes.size() - 1;
}

void DrawShape(HDC hdc, const Shape& shape) {
	HBRUSH brush = CreateSolidBrush(shape.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	HPEN pen;
	if (shape.selected) {
		pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // 검정색 테두리
	}
	else {
		pen = (HPEN)GetStockObject(NULL_PEN); // 윤곽선 없음
	}
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	int px = shape.x * CELL_SIZE;
	int py = shape.y * CELL_SIZE;
	int sz = shape.size;

	switch (shape.type) {
	case CIRCLE:
		Ellipse(hdc, px, py, px + sz, py + sz);
		break;

	case TRIANGLE:
	{
		POINT pts[3] = {
			{ px + sz / 2, py },
			{ px, py + sz },
			{ px + sz, py + sz }
		};
		Polygon(hdc, pts, 3);
	}
	break;

	case RECTANGLE:
		Rectangle(hdc, px, py, px + sz, py + sz);
		break;

	case STAR: {
		POINT pts[10];
		int cx = px + sz / 2;
		int cy = py + sz / 2;
		double angle = -90;

		for (int i = 0; i < 10; ++i) {
			double rad = angle * 3.141592 / 180;
			int r = (i % 2 == 0) ? sz / 2 : sz / 4;
			pts[i].x = cx + (int)(r * cos(rad));
			pts[i].y = cy + (int)(r * sin(rad));
			angle += 36;
		}
		Polygon(hdc, pts, 10);
		break;
	}
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	SelectObject(hdc, oldPen);
	if (shape.selected)
		DeleteObject(pen);
}
