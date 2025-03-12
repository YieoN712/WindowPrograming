#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <sstream>

bool checkNumber(int random);
void generateNum(int num);
void sortNum(char c, bool& sort);

std::default_random_engine dre;
std::uniform_int_distribution<unsigned int> uid{ 0, 100 };

int result[31] = { 0, };
int sorted[31] = { 0, };
bool sortedA = true, sortedD = true;

int main()
{
	int num;

	std::cout << "Number(50~100): ";
	std::cin >> num;
	std::cin.ignore();
	generateNum(num);

	while (true) {
		std::string command;
		std::cout << "command: ";
		std::getline(std::cin, command);
		std::istringstream iss(command.substr(1));

		int rnum, min, max;
		int index, sum = 0, count = 0;

		switch (command[0])
		{
		case '+':
			while (iss >> index) {
				if (count >= 5) {
					break;
				}

				sum += result[index];
				count++;
			}

			std::cout << "Result: " << sum << '\n';
			break;

		case '-':
			if (iss >> index) {
				sum = result[index];
				count++;
			}

			while (iss >> index) {
				if (count >= 5) {
					break;
				}

				sum -= result[index];
				count++;
			}

			std::cout << "Result: " << sum << '\n';
			break;

		case 'a':
			sortNum(command[0], sortedA);
			break;

		case 'd':
			sortNum(command[0], sortedD);
			break;

		case 'r':
			if (iss >> index) {
				for (int i = 0; i < 30; i++) {
					if (result[i] % index == 0) {
						std::cout << result[i] << " ";
					}
				}
			}

			std::cout << "\n";
			break;

		case'm':
			std::cout << "Max: " << *std::max_element(result, result + 30) << "\n";
			break;

		case'n':
			std::cout << "Min: " << *std::min_element(result, result + 30) << "\n";
			break;

		case 's':
			std::cout << "New Number(50~100): ";
			std::cin >> num;
			std::cin.ignore();

			generateNum(num);
			break;

		case 'q':
			std::cout << "Exiting program.\n";
			exit(0);
			break;

		default:
			std::cout << "Invalid command!\n";
			break;
		}
	}
}

bool checkNumber(int random) {
	for (int i = 0; i < 30; i++) {
		if (random == result[i])
			return false;
	}

	return true;
}

void generateNum(int num) {
	int rdNum;

	for (int i = 0; i < 30; i++) {
		while (true) {
			rdNum = uid(dre);

			if (rdNum > num) continue;
			if (not checkNumber(rdNum)) continue;

			result[i] = rdNum;
			sorted[i] = rdNum;
			break;
		}
	}

	for (int i = 0; i < 30; i++) {
		std::cout << result[i] << ' ';
	}
	std::cout << "\n";
}

void sortNum(char c, bool& sort) {
	if (sort) {
		if (c == 'a') {
			std::sort(sorted, sorted + 30);
		}
		else {
			std::sort(sorted, sorted + 30, std::greater<int>());
		}

		sort = false;
	}
	else {
		std::copy(result, result + 30, sorted);
		sort = true;
	}

	for (int i = 0; i < 30; i++) {
		std::cout << sorted[i] << " ";
	}
	std::cout << '\n';
}