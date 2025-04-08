#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <vector>

#define BOARD_SIZE 40
#define CELL_SIZE 20
#define WINDOW_WIDTH (BOARD_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * CELL_SIZE)

enum Shape { SHAPE_TRI, SHAPE_RECT, SHAPE_CIRCLE, SHAPE_ELLIPSE };
enum CellType { CELL_EMPTY, CELL_OBSTACLE, CELL_COLOR, CELL_SIZE_UP, CELL_SIZE_DOWN, CELL_SHAPE };

struct Player {
	int x, y;
	COLORREF color;
	int size;
	Shape shape;
};

const COLORREF COLORS[7] = {
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256),
	RGB(rand() % 256, rand() % 256, rand() % 256)
};

Player player1, player2;
Player target;
int turn = 1; // 1: player1, 2: player2
CellType board[BOARD_SIZE][BOARD_SIZE];

HINSTANCE g_hInst;
LPCTSTR lpszClass = _T("BoardGame");
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawBoard(HDC hdc) {
	HPEN gridPen = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
	HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

	// 세로선 그리기
	for (int i = 0; i <= BOARD_SIZE; i++) {
		MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
		LineTo(hdc, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);
	}

	// 가로선 그리기
	for (int i = 0; i <= BOARD_SIZE; i++) {
		MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
		LineTo(hdc, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(gridPen);
}

COLORREF GetRandomColor() {
	return COLORS[rand() % 7];
}

void InitGame() {
	srand((unsigned)time(NULL));

	memset(board, 0, sizeof(board));

	int count = rand() % 15 + 20;

	// 장애물 칸, 색상 변경, 크기 변경, 모양 변경 칸 무작위 배치
	for (int i = 0; i < count; i++) {
		int x = rand() % BOARD_SIZE;
		int y = rand() % BOARD_SIZE;
		board[y][x] = static_cast<CellType>((rand() % 5) + 1);
	}

	// 플레이어 초기화
	player1 = { 0, 0, GetRandomColor(), 2, SHAPE_RECT };
	player2 = { BOARD_SIZE - 1, 0, GetRandomColor(), 2, SHAPE_CIRCLE };

	// 목표 위치 설정
	target = { BOARD_SIZE / 2, BOARD_SIZE - 1,
		GetRandomColor(),
		rand() % 4 + 1,
		static_cast<Shape>(rand() % 4)
	};
}

void DrawCell(HDC hdc, int x, int y, COLORREF color) {
	RECT rect = { x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE };
	HBRUSH brush = CreateSolidBrush(color);

	FillRect(hdc, &rect, brush);
	DeleteObject(brush);
}

void DrawPlayer(HDC hdc, const Player& p) {
	int cx = p.x * CELL_SIZE;
	int cy = p.y * CELL_SIZE;

	int drawSize = (int)(CELL_SIZE * 0.3f * p.size);
	int offset = (CELL_SIZE - drawSize) / 2;

	HBRUSH brush = CreateSolidBrush(p.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	switch (p.shape) {
	case SHAPE_RECT:
		Rectangle(hdc, cx + offset, cy + offset, cx + offset + drawSize, cy + offset + drawSize);
		break;

	case SHAPE_TRI:
	{
		POINT tri[3] = {
			{cx + CELL_SIZE / 2, cy + offset},
			{cx + offset, cy + offset + drawSize},
			{cx + offset + drawSize, cy + offset + drawSize}
		};
		Polygon(hdc, tri, 3);
	}
	break;

	case SHAPE_CIRCLE:
		Ellipse(hdc, cx + offset, cy + offset, cx + offset + drawSize, cy + offset + drawSize);
		break;

	case SHAPE_ELLIPSE:
		Ellipse(hdc, cx + offset, cy + offset + drawSize / 4, cx + offset + drawSize, cy + offset + 3 * drawSize / 4);
		break;
	}

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}


bool CheckWin(const Player& p) {
	return p.x == target.x && p.y == target.y &&
		p.color == target.color &&
		p.size == target.size &&
		p.shape == target.shape;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASS WndClass = { 0 };

	g_hInst = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszClassName = lpszClass;
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, _T("실습 2-12 게임"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH + 16, WINDOW_HEIGHT + 39,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	InitGame();

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void MovePlayer(Player& p, int dx, int dy) {
	int nx = p.x + dx;
	int ny = p.y + dy;

	if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
		if (board[ny][nx] != CELL_OBSTACLE) {
			p.x = nx;
			p.y = ny;

			switch (board[ny][nx]) {
			case CELL_COLOR:
				p.color = GetRandomColor();
				break;

			case CELL_SIZE_UP:
				if (p.size < 4) p.size++;
				break;

			case CELL_SIZE_DOWN:
				if (p.size > 1) p.size--;
				break;

			case CELL_SHAPE:
				p.shape = static_cast<Shape>((p.shape + 1) % 4);
				break;
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int y = 0; y < BOARD_SIZE; y++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				COLORREF color = RGB(255, 255, 255);
				switch (board[y][x]) {
				case CELL_OBSTACLE: color = RGB(255, 0, 0); break;
				case CELL_COLOR: color = RGB(255, 0, 255); break;
				case CELL_SIZE_UP: color = RGB(0, 255, 0); break;
				case CELL_SIZE_DOWN: color = RGB(0, 100, 0); break;
				case CELL_SHAPE: color = RGB(255, 255, 0); break;
				}
				DrawCell(hdc, x, y, color);
			}
		}

		DrawBoard(hdc);

		DrawPlayer(hdc, player1);
		DrawPlayer(hdc, player2);
		DrawPlayer(hdc, target);

		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'W': if (turn == 1) { MovePlayer(player1, 0, -1); /*turn = 2;*/ } break;
		case 'S': if (turn == 1) { MovePlayer(player1, 0, 1);  /*turn = 2;*/ } break;
		case 'A': if (turn == 1) { MovePlayer(player1, -1, 0); /*turn = 2;*/ } break;
		case 'D': if (turn == 1) { MovePlayer(player1, 1, 0);  /*turn = 2;*/ } break;
		case 'I': if (turn == 2) { MovePlayer(player2, 0, -1); /*turn = 1;*/ } break;
		case 'K': if (turn == 2) { MovePlayer(player2, 0, 1);  /*turn = 1;*/ } break;
		case 'J': if (turn == 2) { MovePlayer(player2, -1, 0); /*turn = 1;*/ } break;
		case 'L': if (turn == 2) { MovePlayer(player2, 1, 0);  /*turn = 1;*/ } break;
		case 'R': InitGame(); break;
		case '1': turn = 1; break;
		case '2': turn = 2; break;
		case 'Q':
			PostQuitMessage(0);
			break;
		}

		if (CheckWin(player1) || CheckWin(player2)) {
			MessageBox(hWnd, CheckWin(player1) ? _T("Player 1 Wins!") : _T("Player 2 Wins!"), _T("Game Over"), MB_OK);
			InitGame();
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
