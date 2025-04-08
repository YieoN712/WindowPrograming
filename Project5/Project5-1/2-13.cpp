#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <vector>

#define BOARD_SIZE 20
#define CELL_SIZE 40
#define WINDOW_WIDTH (BOARD_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * CELL_SIZE)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBoard(HDC);
void DrawBlocks(HDC);
void DrawPlayer(HDC);
void DrawPuzzle(HDC, HWND);
void MovePlayer(int, int);
void ResetGame();
void CheckPuzzle();
TCHAR CombineLetters(TCHAR, TCHAR);

HINSTANCE g_hInst;
LPCTSTR lpszClass = _T("BlockGame");

struct Block {
	int x, y;
	TCHAR ch;
	bool active;
};

struct Player {
	int x = BOARD_SIZE / 2;
	int y = BOARD_SIZE - 1;
};

const TCHAR* wordList[] = {
	_T("APPLE"),
	_T("WINDOW"),
	_T("HOUSE"),
	_T("COMPUTER"),
	_T("GAMES")
};
const int wordCount = sizeof(wordList) / sizeof(wordList[0]);

std::vector<Block> blocks;
Player player;

TCHAR completed[21];
TCHAR puzzle[21];
bool puzzleComplete = false;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpszCmd, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, _T("실습 2-13 게임"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH + 16, WINDOW_HEIGHT + 39,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	srand((unsigned int)time(NULL));
	ResetGame();

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void ResetGame() {
	blocks.clear();
	player = { BOARD_SIZE / 2, BOARD_SIZE - 1 };
	puzzleComplete = false;

	int idx = rand() % wordCount;
	_tcscpy_s(completed, wordList[idx]);
	_tcscpy_s(puzzle, completed);

	int len = lstrlen(puzzle);
	int holes = 1 + rand() % 3;

	for (int i = 0; i < holes; ++i) {
		int pos;
		do {
			pos = rand() % len;
		} while (puzzle[pos] == _T('_'));
		puzzle[pos] = _T('_');
	}

	bool used[BOARD_SIZE][BOARD_SIZE] = { false };

	for (int i = 0; i < 52; ) {
		int x = rand() % BOARD_SIZE;
		int y = 3 + rand() % (BOARD_SIZE - 6);	// 거리 확보

		if (used[y][x]) continue;

		Block b;
		b.x = x;
		b.y = y;
		b.ch = _T('A') + (i % 26);
		b.active = true;
		blocks.push_back(b);
		used[y][x] = true;
		i++;
	}
}


void DrawBoard(HDC hdc) {
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	for (int i = 0; i <= BOARD_SIZE; i++) {
		MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
		LineTo(hdc, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);
		MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
		LineTo(hdc, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
	}

	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}

void DrawPuzzle(HDC hdc, HWND hwnd) {
	HFONT hFont, hOldFont;
	int fontSize = CELL_SIZE * 0.6;

	hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("Arial"));

	hOldFont = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT);

	int len = lstrlen(puzzle);
	int startX = (BOARD_SIZE - len) / 2;

	for (int i = 0; i < len; ++i) {
		if (puzzle[i] == _T('_')) continue; // 빈칸이면 출력 X

		TCHAR chStr[2] = { puzzle[i], 0 };
		RECT cell = {
			(startX + i) * CELL_SIZE,
			1 * CELL_SIZE,  // 보드 두 번째 줄
			(startX + i + 1) * CELL_SIZE,
			2 * CELL_SIZE
		};

		DrawText(hdc, chStr, 1, &cell, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// "완성!" 출력
	if (puzzleComplete) {
		MessageBox(hwnd, _T("퍼즐이 완성되었습니다!"), _T("축하합니다"), MB_OK);
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawBlocks(HDC hdc) {
	HFONT hFont, hOldFont;
	int fontSize = CELL_SIZE * 0.6;  // 셀 크기에 비례해서 폰트 크기 설정
	hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("Arial"));

	hOldFont = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(100, 100, 255));
	SetBkMode(hdc, TRANSPARENT);

	for (auto& b : blocks) {
		if (!b.active) continue;
		TCHAR chStr[2] = { b.ch, 0 };

		RECT cell = {
			b.x * CELL_SIZE,
			b.y * CELL_SIZE,
			(b.x + 1) * CELL_SIZE,
			(b.y + 1) * CELL_SIZE
		};

		DrawText(hdc, chStr, 1, &cell, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}


void DrawPlayer(HDC hdc) {
	HBRUSH hBrush = CreateSolidBrush(RGB(100, 200, 255));
	RECT rect = {
		player.x * CELL_SIZE,
		player.y * CELL_SIZE,
		(player.x + 1) * CELL_SIZE,
		(player.y + 1) * CELL_SIZE
	};
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

void MovePlayer(int dx, int dy) {
	int newX = player.x + dx;
	int newY = player.y + dy;

	if (newX < 0 || newX >= BOARD_SIZE || newY < 0 || newY >= BOARD_SIZE)
		return;

	// 블록이 있는지 확인
	for (auto& b : blocks) {
		if (b.active && b.x == newX && b.y == newY) {
			int bx = b.x + dx;
			int by = b.y + dy;
			if (bx < 0 || bx >= BOARD_SIZE || by < 0 || by >= BOARD_SIZE)
				return;

			// 밀려나는 위치에 또 블록이 있으면 합치기 시도
			for (auto& b2 : blocks) {
				if (b2.active && b2.x == bx && b2.y == by) {
					b2.ch = CombineLetters(b.ch, b2.ch);
					b.active = false;
					goto moved;
				}
			}

			// 밀 수 있으면 이동
			b.x = bx;
			b.y = by;
			break;
		}
	}

moved:
	player.x = newX;
	player.y = newY;

	// 단어 완성 확인
	CheckPuzzle();
}

TCHAR CombineLetters(TCHAR a, TCHAR b) {
	int sum = ((a - 'A' + 1) + (b - 'A' + 1)) % 26;
	if (sum == 0) sum = 26;
	return _T('A' + sum - 1);
}

void CheckPuzzle() {
	int len = lstrlen(puzzle);
	int startX = (BOARD_SIZE - len) / 2;

	for (int i = 0; i < len; ++i) {
		if (puzzle[i] == _T('_')) {
			for (auto& b : blocks) {
				if (b.active && b.y == 1 && b.x == (startX + i)) {
					if (b.ch == completed[i]) {
						puzzle[i] = b.ch;
					}
				}
			}
		}
	}

	puzzleComplete = (_tcscmp(puzzle, completed) == 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBoard(hdc);
		DrawPuzzle(hdc, hWnd);
		DrawBlocks(hdc);
		DrawPlayer(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT: MovePlayer(-1, 0); break;
		case VK_RIGHT: MovePlayer(1, 0); break;
		case VK_UP: MovePlayer(0, -1); break;
		case VK_DOWN: MovePlayer(0, 1); break;
		case 'S': ResetGame(); break;
		case 'Q': PostQuitMessage(0); break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
