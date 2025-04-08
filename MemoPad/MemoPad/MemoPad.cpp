#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#define MAX_LINE 10
#define MAX_CHAR 30

TCHAR buffer[MAX_LINE][MAX_CHAR + 1] = { 0 };
HFONT hFont;
TCHAR replaceTargetChar = 0;
int curLine = 0, curChar = 0;
bool insertMode = true;
bool upperCase = false;
bool markDigits = false;
bool wrapWords = false;
bool removeSpaces = false;
bool replaceRandom = false;
bool shifted = false;
bool reversed = false;
int prevX = 0;
int fontSize = 18;
int LINE_HEIGHT = 18;

std::vector<int> randomCharIndices;

void MoveCaret(HWND);
void ShiftLines();

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
	HFONT hOldFont;

	switch (iMessage)
	{
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 1, LINE_HEIGHT);
		ShowCaret(hWnd);
		MoveCaret(hWnd);
		srand((unsigned)time(NULL));

		hFont = CreateFont(
			fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_MODERN, _T("Consolas"));
		break;

	case WM_CHAR:
		if (wParam == VK_RETURN) {
			curLine = (curLine + 1) % MAX_LINE;
			curChar = 0;
		}
		else if (wParam == VK_BACK) {
			if (curChar > 0) {
				int len = _tcslen(buffer[curLine]);
				for (int i = curChar - 1; i < len; i++) {
					buffer[curLine][i] = buffer[curLine][i + 1]; // 뒤에 걸 앞으로 당김
				}
				curChar--; // 커서 왼쪽으로 이동
			}
			else if (curLine > 0) {
				// 이전 줄로 이동
				curLine--;
				curChar = _tcslen(buffer[curLine]);
			}
		}
		else if (_istprint(wParam)) {
			if (wParam == '=' or wParam == '-') break;  // 화면에 표시하지 않음

			TCHAR ch = (TCHAR)(upperCase ? _totupper(wParam) : wParam);

			if (insertMode) {
				// 삽입 모드
				if (_tcslen(buffer[curLine]) < MAX_CHAR) {
					for (int i = _tcslen(buffer[curLine]); i > curChar; i--) {
						buffer[curLine][i] = buffer[curLine][i - 1];
					}
					buffer[curLine][curChar++] = ch;
				}
				else if (curLine < MAX_LINE - 1) {
					// 줄이 가득 찼으면 다음 줄로 이동
					curLine++;
					curChar = 0;
					if (_tcslen(buffer[curLine]) < MAX_CHAR) {
						for (int i = _tcslen(buffer[curLine]); i > curChar; i--) {
							buffer[curLine][i] = buffer[curLine][i - 1];
						}
						buffer[curLine][curChar++] = ch;
					}
				}
			}
			else {
				// 덮어쓰기 모드
				if (curChar < MAX_CHAR) {
					buffer[curLine][curChar++] = ch;
				}
				else if (curLine < MAX_LINE - 1) {
					// 줄이 꽉 차면 다음 줄로
					curLine++;
					curChar = 0;
					buffer[curLine][curChar++] = ch;
				}
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_INSERT: insertMode = !insertMode; break;
		case VK_HOME: curChar = 0; break;
		case VK_END: curChar = _tcslen(buffer[curLine]); break;
		case VK_TAB:
			if (curChar + 5 <= MAX_CHAR) {
				for (int i = 0; i < 5 && curChar < MAX_CHAR; i++) {
					buffer[curLine][curChar++] = ' ';
				}
			}
			else if (curLine < MAX_LINE - 1) {
				curLine++;
				curChar = 0;
			}
			break;
		case VK_ESCAPE:
			ZeroMemory(buffer, sizeof(buffer));
			curLine = curChar = 0;
			break;
		case VK_DELETE:
		{
			int start = curChar;
			while (buffer[curLine][start] != ' ' && buffer[curLine][start] != '\0') {
				for (int i = start; i < MAX_CHAR; i++)
					buffer[curLine][i] = buffer[curLine][i + 1];
			}
		}
		break;
		case VK_LEFT: if (curChar > 0) curChar--; break;
		case VK_RIGHT: if (curChar < lstrlen(buffer[curLine])) curChar++; break;
		case VK_UP:
			curLine = (curLine - 1 + MAX_LINE) % MAX_LINE;
			curChar = min(curChar, _tcslen(buffer[curLine]));
			break;
		case VK_DOWN:
			curLine = (curLine + 1) % MAX_LINE;
			curChar = min(curChar, _tcslen(buffer[curLine]));
			break;
		case VK_PRIOR:
			curLine = (curLine - 3 + MAX_LINE) % MAX_LINE;
			curChar = min(curChar, _tcslen(buffer[curLine]));
			break;
		case VK_NEXT:
			curLine = (curLine + 3) % MAX_LINE;
			curChar = min(curChar, _tcslen(buffer[curLine]));
			break;
		case VK_F1: upperCase = !upperCase; break;
		case VK_F2: markDigits = !markDigits; break;
		case VK_F3: wrapWords = !wrapWords; break;
		case VK_F4: removeSpaces = !removeSpaces; break;
		case VK_F5:
			replaceRandom = !replaceRandom;
			replaceTargetChar = 0;

			if (replaceRandom) {
				std::vector<TCHAR> allChars;

				// buffer 전체에서 인쇄 가능한 문자 수집
				for (int i = 0; i < MAX_LINE; i++) {
					for (int j = 0; j < _tcslen(buffer[i]); j++) {
						if (_istgraph(buffer[i][j])) {
							allChars.push_back(buffer[i][j]);
						}
					}
				}

				// 하나라도 있으면 랜덤 선택
				if (!allChars.empty()) {
					replaceTargetChar = allChars[rand() % allChars.size()];
				}
			}
			break;
		case VK_F6: ShiftLines(); break;
		case VK_F11:  // 줄이기
			if (fontSize > 5) fontSize--;
			goto UpdateFont;

		case VK_F12:  // 늘리기
			fontSize++;
			goto UpdateFont;

		UpdateFont:
			if (hFont) DeleteObject(hFont);
			hFont = CreateFont(
				fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_MODERN, _T("Consolas"));

			LINE_HEIGHT = fontSize + 2;  // 글꼴 높이 기준
			DestroyCaret();
			CreateCaret(hWnd, NULL, 2, LINE_HEIGHT);
			ShowCaret(hWnd);
			MoveCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_OEM_PLUS:
			// 모든 문자 +1 처리
			for (int i = 0; i < MAX_LINE; i++) {
				int len = _tcslen(buffer[i]);
				TCHAR newLine[MAX_CHAR + 1] = { 0 };
				int idx = 0;

				for (int j = 0; j < len;) {
					if (_istalpha(buffer[i][j])) {
						// 알파벳 증가
						TCHAR ch = buffer[i][j];
						if (ch == 'z') ch = 'a';
						else if (ch == 'Z') ch = 'A';
						else ch++;

						newLine[idx++] = ch;
						j++;
					}
					else if (_istdigit(buffer[i][j])) {
						// 숫자 덩어리 처리
						TCHAR numStr[10] = { 0 };
						int numIdx = 0;
						while (j < len && _istdigit(buffer[i][j]) && numIdx < 9) {
							numStr[numIdx++] = buffer[i][j++];
						}
						int value = _ttoi(numStr);
						value += 1;

						TCHAR temp[10];
						_itot(value, temp, 10);

						for (int k = 0; k < _tcslen(temp) && idx < MAX_CHAR; k++) {
							newLine[idx++] = temp[k];
						}
					}
					else {
						// 기타 문자 그대로
						newLine[idx++] = buffer[i][j++];
					}
				}

				newLine[idx] = '\0';
				_tcscpy(buffer[i], newLine);
			}
			break;
		case VK_OEM_MINUS:
			// 모든 문자 -처리
			for (int i = 0; i < MAX_LINE; i++) {
				int len = _tcslen(buffer[i]);
				TCHAR newLine[MAX_CHAR + 1] = { 0 };
				int idx = 0;

				for (int j = 0; j < len;) {
					if (_istalpha(buffer[i][j])) {
						// 알파벳 감소
						TCHAR ch = buffer[i][j];
						if (ch == 'a') ch = 'z';
						else if (ch == 'A') ch = 'Z';
						else ch--;

						newLine[idx++] = ch;
						j++;
					}
					else if (_istdigit(buffer[i][j])) {
						// 숫자 덩어리 처리
						TCHAR numStr[10] = { 0 };
						int numIdx = 0;
						while (j < len && _istdigit(buffer[i][j]) && numIdx < 9) {
							numStr[numIdx++] = buffer[i][j++];
						}
						int value = _ttoi(numStr);
						value = max(0, value - 1);  // 음수 방지

						TCHAR temp[10];
						_itot(value, temp, 10);

						for (int k = 0; k < _tcslen(temp) && idx < MAX_CHAR; k++) {
							newLine[idx++] = temp[k];
						}
					}
					else {
						newLine[idx++] = buffer[i][j++];
					}
				}

				newLine[idx] = '\0';
				_tcscpy(buffer[i], newLine);
			}
			break;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hFont);

		for (int i = 0; i < MAX_LINE; i++) {
			TCHAR temp[MAX_CHAR + 1];
			_tcscpy(temp, buffer[i]);
			int len = lstrlen(temp);

			// Shift or Reverse
			if (shifted || reversed) {
				for (int j = 0; j < len; j++) {
					if (_istalpha(temp[j])) {
						if (shifted) temp[j] = (temp[j] == 'z' || temp[j] == 'Z') ? temp[j] - 25 : temp[j] + 1;
						else if (reversed) temp[j] = (temp[j] == 'a' || temp[j] == 'A') ? temp[j] + 25 : temp[j] - 1;
					}
					else if (_istdigit(temp[j])) {
						if (shifted) temp[j] = (temp[j] == '9') ? '0' : temp[j] + 1;
						else if (reversed) temp[j] = (temp[j] == '0') ? '9' : temp[j] - 1;
					}
				}
			}

			// Remove spaces
			if (removeSpaces) {
				int idx = 0;
				for (int j = 0; j < len; j++) {
					if (temp[j] != ' ') temp[idx++] = temp[j];
				}
				temp[idx] = '\0';
				len = idx;
			}

			// Wrap words
			if (wrapWords) {
				TCHAR wrapped[MAX_CHAR * 2 + 1] = { 0 };
				TCHAR* token = _tcstok(temp, _T(" "));
				while (token) {
					TCHAR word[MAX_CHAR + 1];
					wsprintf(word, _T("(%s)"), _tcsupr(token));
					_tcscat(wrapped, word);
					token = _tcstok(NULL, _T(" "));
				}
				_tcscpy(temp, wrapped);
				len = lstrlen(temp);
			}

			// Replace random chars
			if (replaceRandom && replaceTargetChar != 0) {
				for (int j = 0; j < len; j++) {
					if (temp[j] == replaceTargetChar) {
						temp[j] = '@';
					}
				}
			}

			// Mark digits
			if (markDigits) {
				bool hasDigit = false;
				for (int j = 0; j < len; j++) {
					if (_istdigit(temp[j])) {
						hasDigit = true;
						break;
					}
				}
				if (hasDigit) {
					TCHAR marked[MAX_CHAR * 2 + 1];
					wsprintf(marked, _T("****%s"), temp);
					_tcscpy(temp, marked);
				}
			}

			// 최종 출력
			TextOut(hDC, 0, i * LINE_HEIGHT, temp, lstrlen(temp));
		}

		SelectObject(hDC, hOldFont);
		MoveCaret(hWnd);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		DeleteObject(hFont);
		HideCaret(hWnd);
		DestroyCaret();
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void MoveCaret(HWND hwnd) {
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, hFont);  // 폰트 적용
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	int charWidth = tm.tmAveCharWidth;

	SetCaretPos(curChar * charWidth, curLine * LINE_HEIGHT);
	ReleaseDC(hwnd, hdc);
}

void ShiftLines() {
	// 현재 입력된 줄 개수 파악
	int lineCount = 0;
	for (int i = 0; i < MAX_LINE; i++) {
		if (_tcslen(buffer[i]) > 0) lineCount++;
		else break;
	}

	// 줄이 2개 이상 있어야 회전 의미 있음
	if (lineCount < 2) return;

	TCHAR temp[MAX_CHAR + 1];
	_tcscpy(temp, buffer[lineCount - 1]);

	for (int i = lineCount - 1; i > 0; i--) {
		_tcscpy(buffer[i], buffer[i - 1]);
	}
	_tcscpy(buffer[0], temp);
}
