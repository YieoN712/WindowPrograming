#include <windows.h>
#include <tchar.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

#define MAX_SHAPES 5
#define MAX_INPUT 100

enum ShapeType { LINE = 2, TRIANGLE, RECTANGLE, PENTAGON, HEXAGON };

struct Shape {
	ShapeType type;
	POINT p1, p2;
	int thickness;
	COLORREF borderColor;
	COLORREF fillColor;
};

std::vector<Shape> shapes;
int currentIndex = -1;

TCHAR inputBuffer[MAX_INPUT] = { 0 };
int inputPos = 0;

HFONT hFont;
Shape tempShape;

void DrawShape(HDC, const Shape&);
void DrawAllShapes(HDC);
void DrawShapeInfo(HDC);
void ParseInput(HWND);

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"ShapeDrawClass";
LPCTSTR lpszWindowName = L"실습 2-15 도형 그리기";

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
	srand((unsigned)time(NULL));

	while (GetMessage(&Message, NULL, 0, 0)) {
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
		hFont = CreateFont(
			20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_MODERN, _T("Consolas"));
		break;

	case WM_CHAR:
		if (wParam == VK_RETURN) {
			ParseInput(hWnd);
		}
		else if (wParam == VK_BACK and inputPos > 0) {
			inputBuffer[--inputPos] = 0;
		}
		else if (_istprint(wParam) and inputPos < MAX_INPUT - 1) {
			inputBuffer[inputPos++] = (TCHAR)wParam;
			inputBuffer[inputPos] = 0;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
	{
		if (!shapes.empty()) {
			Shape& s = shapes[currentIndex];

			switch (wParam)
			{
			case VK_UP: s.p1.y -= 10; s.p2.y -= 10; break;
			case VK_DOWN: s.p1.y += 10; s.p2.y += 10; break;
			case VK_LEFT: s.p1.x -= 10; s.p2.x -= 10; break;
			case VK_RIGHT: s.p1.x += 10; s.p2.x += 10; break;
			case '+': case '=':
				if (s.thickness < 10) s.thickness++;
				break;
			case '-':
				if (s.thickness > 1) s.thickness--;
				break;
			case '1':
				s.borderColor = RGB(rand() % 256, rand() % 256, rand() % 256);
				break;
			case '2':
				s.fillColor = RGB(rand() % 256, rand() % 256, rand() % 256);
				break;
			case 'P': case 'p':
				if (currentIndex > 0) currentIndex--;
				break;
			case 'N': case 'n':
				if (currentIndex < (int)shapes.size() - 1) currentIndex++;
				break;
			case 'A': case 'a':
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case 'D': case 'd':
				shapes.clear();
				currentIndex = -1;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case 'Q': case 'q':
				PostQuitMessage(0);
				break;
			}
		}
		else {
			MessageBox(hWnd, _T("EMPTY"), _T("ERROR"), MB_OK);
			break;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SelectObject(hDC, hFont);
		TextOut(hDC, 10, 10, _T("입력: "), 4);
		TextOut(hDC, 70, 10, inputBuffer, _tcslen(inputBuffer));

		DrawAllShapes(hDC);
		DrawShapeInfo(hDC);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawShape(HDC hdc, const Shape& s) {
	HPEN pen = CreatePen(PS_SOLID, s.thickness, s.borderColor);
	HBRUSH brush = CreateSolidBrush(s.fillColor);

	SelectObject(hdc, pen);
	SelectObject(hdc, brush);

	int w = s.p2.x - s.p1.x;
	int h = s.p2.y - s.p1.y;
	int cx = (s.p1.x + s.p2.x) / 2;
	int cy = (s.p1.y + s.p2.y) / 2;

	switch (s.type)
	{
	case LINE:
		MoveToEx(hdc, s.p1.x, s.p1.y, NULL);
		LineTo(hdc, s.p2.x, s.p2.y);
		break;

	case TRIANGLE:
	{
		POINT pts[3] = { {cx, s.p1.y}, {s.p1.x, s.p2.y},{s.p2.x,s.p2.y} };
		Polygon(hdc, pts, 3);
		break;
	}

	case RECTANGLE:
		Rectangle(hdc, s.p1.x, s.p1.y, s.p2.x, s.p2.y);
		break;

	case PENTAGON:
	{
		POINT pts[5];

		for (int i = 0; i < 5; ++i) {
			double angle = 72 * i * 3.141592 / 180;
			pts[i].x = cx + (int)(w / 2 * cos(angle));
			pts[i].y = cy + (int)(h / 2 * sin(angle));
		}

		Polygon(hdc, pts, 5);
		break;
	}

	case HEXAGON:
	{
		POINT pts[6];

		for (int i = 0; i < 6; ++i) {
			double angle = 60 * i * 3.141592 / 180;
			pts[i].x = cx + (int)(w / 2 * cos(angle));
			pts[i].y = cy + (int)(h / 2 * sin(angle));
		}

		Polygon(hdc, pts, 6);
		break;
	}
	}

	DeleteObject(pen);
	DeleteObject(brush);
}

void DrawAllShapes(HDC hdc) {
	for (const auto& s : shapes) {
		DrawShape(hdc, s);
	}
}

void DrawShapeInfo(HDC hdc) {
	int x = 550, y = 40;

	for (size_t i = 0; i < shapes.size(); ++i) {
		TCHAR buf[256];
		_stprintf_s(buf, _T("[%d] Type: %d P1(%d, %d) P2(%d, %d) T: %d"),
			(int)i, shapes[i].type, shapes[i].p1.x, shapes[i].p1.y,
			shapes[i].p2.x, shapes[i].p2.y, shapes[i].thickness);
		TextOut(hdc, x, y + (int)i * 20, buf, _tcslen(buf));
	}
}

void ParseInput(HWND hWnd) {
	int values[6] = { 0 };
	int count = 0;

	TCHAR* context = NULL;
	TCHAR* token = _tcstok_s(inputBuffer, _T(" "), &context);

	while (token != NULL && count < 6) {
		values[count++] = _ttoi(token);
		token = _tcstok_s(NULL, _T(" "), &context);
	}

	if (count < 6) {
		MessageBox(hWnd, _T("입력값이 부족합니다. 예: 2 100 100 200 200 3"), _T("에러"), MB_OK);
		return;
	}

	if (values[0] < 2 || values[0] > 6) {
		MessageBox(hWnd, _T("도형 종류는 2~6이어야 합니다."), _T("에러"), MB_OK);
		return;
	}

	if (values[5] < 1 || values[5] > 10) {
		MessageBox(hWnd, _T("두께는 1~10이어야 합니다."), _T("에러"), MB_OK);
		return;
	}

	Shape newShape;
	newShape.type = (ShapeType)values[0];
	newShape.p1.x = values[1];
	newShape.p1.y = values[2];
	newShape.p2.x = values[3];
	newShape.p2.y = values[4];
	newShape.thickness = values[5];
	newShape.borderColor = RGB(rand() % 256, rand() % 256, rand() % 256);
	newShape.fillColor = RGB(rand() % 256, rand() % 256, rand() % 256);

	if ((int)shapes.size() < MAX_SHAPES) {
		shapes.push_back(newShape);
		currentIndex = (int)shapes.size() - 1;
	}
	else {
		MessageBox(hWnd, _T("최대 5개의 도형만 입력할 수 있습니다."), _T("제한"), MB_OK);
	}

	// 입력 초기화
	inputPos = 0;
	inputBuffer[0] = 0;

	InvalidateRect(hWnd, NULL, TRUE);
}