#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <conio.h>
#include <windows.h>

#define row 13
#define col 10
#define alphabetSet 5

char board[row][col];
char original[row][col];
int curX = 0, curY = 0;
char target = ' ';
int count = 0;

void gotoxy(int x, int y);
void setColor(int color);
void resetBoard();
void printBoard();
void move(int dir);
void selectChar();
void shuffle();
void ending();

int main()
{
	resetBoard();
	shuffle();

	char command;

	while (true) {
		if (count == 5) {
			ending();
			break;
		}

		std::cout << "command: ";
		command = _getch();

		if (command >= 'a' and command <= 'z') {
			target = command;
			printBoard();
		}
		else if (command == '1' or command == '2' or command == '3' or command == '4') {
			move(command - '0');
		}
		else if (command == '\r') {
			selectChar();
		}
		else if (command == '5') {
			shuffle();
		}
		else if (command == '9') {
			resetBoard();
			printBoard();
		}
		else if (command == '0') {
			std::cout << "game exit\n\n";
			break;
		}
		else {
			std::cout << "wrong command\n";
			continue;
		}
	}

}

void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetBoard() {
	std::vector<char> letter;

	for (int i = 0; i < alphabetSet; i++) {
		for (char c = 'a'; c <= 'z'; c++) {
			letter.push_back(c);
		}
	}

	int index = 0;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			board[r][c] = letter[index++];
		}
	}

	std::memcpy(original, board, sizeof(board));
}

void printBoard() {
	system("cls");

	std::cout << "target: ";
	if (target != ' ') {
		setColor(10);
		std::cout << target;
		setColor(7);
	}
	std::cout << "\n\n";

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			if (board[r][c] == 'O') {
				setColor(12);
				std::cout << board[r][c] << " ";
				setColor(7);
			}
			else if (r == curX and c == curY) {
				setColor(12);
				std::cout << board[r][c] << " ";
				setColor(7);
			}
			else {
				std::cout << board[r][c] << " ";
			}
		}
		std::cout << '\n';
	}

	std::cout << "location: (" << curX + 1 << "," << curY + 1 << ")\n";
}

void move(int dir) {
	switch (dir)
	{
	case 1:
		if (curY > 0)
			curY--;
		break;

	case 2:
		if (curY < col - 1)
			curY++;
		break;

	case 3:
		if (curX > 0)
			curX--;
		break;

	case 4:
		if (curX < row - 1)
			curX++;
		break;
	}

	printBoard();
}

void selectChar() {
	if (target == ' ') return;

	if (board[curX][curY] == target) {
		board[curX][curY] = 'O';
		count++;
		printBoard();
	}
	else {
		std::cout << "\a";
	}
}

void shuffle() {
	std::vector<char> letter;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			if (board[r][c] != 'O') {
				letter.push_back(board[r][c]);
			}
		}
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(letter.begin(), letter.end(), gen);

	int index = 0;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			if (board[r][c] != 'O') {
				board[r][c] = letter[index++];
			}
		}
	}

	printBoard();
}

void ending() {
	system("cls");

	std::cout << "target: ";
	if (target != ' ') {
		setColor(10);
		std::cout << target;
		setColor(7);
	}
	std::cout << "\n\n";

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			if (board[r][c] == 'O') {
				setColor(9);
				std::cout << board[r][c] << " ";
				setColor(7);
			}
			else {
				std::cout << board[r][c] << " ";
			}
		}
		std::cout << '\n';
	}

	setColor(14);
	std::cout << "\nCLEAR!!!!\n";
	setColor(7);
}